#include "yocto/threading/scheme/server.hpp"

namespace yocto
{
    namespace threading
    {
        server:: ~server() throw()
        {
        }

        server:: server( executor &xxx ) throw() :
        cpu(xxx),
        juuid(0)
        {
        }
        
    }
}

namespace yocto
{
    namespace threading
    {
        seq_server:: seq_server() :
        seq_executor(),
        server( *static_cast<seq_executor *>(this) )
        {
        }

        seq_server:: ~seq_server() throw()
        {
        }

        job_id seq_server:: enqueue( kernel &k )
        {
            k(ctx);
            return ++juuid;
        }
    }
}


namespace yocto
{
    namespace threading
    {

        static const char fn[] = "[threads server] ";

        par_server:: task:: ~task() throw()
        {
        }

        par_server:: task:: task(const job_id j, const kernel &k) :
        next(0),
        prev(0),
        uuid(j),
        proc(k)
        {
        }

        par_server:: ~par_server() throw()
        {
            quit();
        }

        par_server:: par_server(const bool setVerbose) :
        par_executor(setVerbose),
        server( *static_cast<par_executor *>(this) ),
        pending(),
        current(),
        storage(),
        ready(0),
        incoming(),
        activity(),
        flushing()
        {
            init();
        }


        void par_server:: quit() throw()
        {
            //__________________________________________________________________
            //
            // Take control
            //__________________________________________________________________
            access.lock();

            //__________________________________________________________________
            //
            // activate the stop flag for everyone
            //__________________________________________________________________
            if(verbose) { std::cerr << fn << "stopping jobs" << std::endl; }
            stopping = true;

            //__________________________________________________________________
            //
            // clean up pending tasks
            //__________________________________________________________________
            if(verbose) { std::cerr << fn << "#pending tasks: " << pending.size << std::endl; }
            while(pending.size)
            {
                task *t = pending.pop_back();
                t->~task();
                object::release1<task>(t);
            }

            //__________________________________________________________________
            //
            // release control for current tasks to finish
            //__________________________________________________________________
            access.unlock();
            flush();

            //__________________________________________________________________
            //
            // everybody come back
            //__________________________________________________________________
            access.lock();
            if(verbose) { std::cerr << fn << "final call" << std::endl; }
            activity.broadcast();
            incoming.broadcast();
            access.unlock();

            //__________________________________________________________________
            //
            // empty storage
            //__________________________________________________________________
            while(storage.size)
            {
                object::release1<task>( storage.query() );
            }

            //__________________________________________________________________
            //
            // threads destructor takes care of cleanup
            //__________________________________________________________________

        }

        void par_server:: flush() throw()
        {
            access.lock();
            if(verbose) { std::cerr << fn << "flushing" << std::endl; }
            if(pending.size>0)
            {
                // wait on a locked mutex
                flushing.wait(access);

                // wake up on a locked mutex
                if(verbose) { std::cerr << fn << "flushed..." << std::endl; }
            }
            access.unlock();
        }


        void par_server:: init()
        {
            try
            {
                //______________________________________________________________
                //
                // launch workers
                //______________________________________________________________
                for(size_t rank=0;rank<size;++rank)
                {
                    context &ctx = (*this)[rank];
                    ctx.priv     = this;
                    workers.launch(start_workers,&ctx);
                }

                //______________________________________________________________
                //
                // launch controler
                //______________________________________________________________
                workers.launch(start_control,this);


                //______________________________________________________________
                //
                // wait for first synchro
                //______________________________________________________________
                while( true )
                {
                    if( access.try_lock() )
                    {
                        if(ready>size)
                        {
                            if(verbose) { std::cerr << fn << "synchronised" << std::endl; }
                            // thread placement:
                            // the control thread is on first allowed CPU !
                            size_t iThread = 0;
                            for(thread *thr = workers.head; thr; thr=thr->next )
                            {
                                thr->on_cpu( cpu_index_of(iThread++) );
                            }
                            access.unlock();
                            return;
                        }
                        else
                        {
                            access.unlock();
                        }
                    }
                }


            }
            catch(...)
            {
                quit();
                throw;
            }
        }

        void par_server:: start_control( void *args ) throw()
        {
            assert(args);
            static_cast<par_server *>(args)->control_loop();
        }

        void par_server:: start_workers(void *args) throw()
        {
            assert(args);
            context &ctx = *static_cast<context *>(args); assert(ctx.priv);
            static_cast<par_server *>(ctx.priv)->workers_loop(ctx);
        }


#define __DISPLAY(CODE) do { if(verbose) { std::cerr << fn << CODE << std::endl; } } while(false)


        //______________________________________________________________________
        //
        //
        //  control loop to activate threads
        //
        //______________________________________________________________________
        void par_server::control_loop() throw()
        {

            access.lock();
            if(verbose)
            {
                std::cerr << fn << "control: enter" << std::endl;
            }
            ++ready;

        WAIT_FOR_ACTIVITY:
            //__________________________________________________________________
            //
            // wait on a LOCKED mutex
            //__________________________________________________________________
            if(verbose) { std::cerr << fn << "control: wait for activity" << std::endl; }
            activity.wait(access);

            //__________________________________________________________________
            //
            // wake up on LOCKED mutex
            //__________________________________________________________________
            if(verbose) { std::cerr << fn << "control: activity detected" << std::endl; }
            if(stopping)
            {
                //______________________________________________________________
                //
                // end of thread
                //______________________________________________________________
                if(verbose) { std::cerr << fn << "control: leave" << std::endl; }
                access.unlock();
                return; // end of thread
            }
            if(verbose) { std::cerr << fn << "control: checking tasks" << std::endl; }

            //__________________________________________________________________
            //
            // somebdy woke me up, let's see why
            //__________________________________________________________________
            if(pending.size)
            {
                // more work
                if(verbose) { std::cerr << fn << "control: signaling incoming..." << std::endl; }
                incoming.signal();
            }
            else
            {
                // if main threads await...
                if(verbose) { std::cerr << fn << "control: signaling flushed..." << std::endl; }
                flushing.broadcast();
            }


            //__________________________________________________________________
            //
            // ok, end of turn, the wait will unlock the mutex
            //__________________________________________________________________
            if(verbose) { std::cerr << fn << "control: waiting for next activity" << std::endl; }
            goto WAIT_FOR_ACTIVITY; // will unlock access

        }


        void par_server::workers_loop(context &ctx) throw()
        {
            access.lock();
            if(verbose) { std::cerr << fn << "workers: enter " << ctx.size << "." << ctx.rank << std::endl; }
            ++ready;

        WAITING_FOR_WORK:
            //__________________________________________________________________
            //
            // wait on a LOCKED mutex
            //__________________________________________________________________
            if(verbose) { std::cerr << fn << "workers: "<< ctx.size << "." << ctx.rank << " waiting for incoming..." << std::endl; }
            incoming.wait(access);

            //__________________________________________________________________
            //
            // wake up on a LOCKED mutex
            //__________________________________________________________________
            if(stopping)
            {
                //__________________________________________________________________
                //
                // end of thread
                //__________________________________________________________________
                if(verbose) { std::cerr << fn << "workers: leave " << ctx.size << "." << ctx.rank << std::endl; }
                access.unlock();
                return;
            }

            if(verbose) { std::cerr << fn << "workers: checking job for " << ctx.size << "." << ctx.rank << std::endl; }
            //__________________________________________________________________
            ///
            // ok, signaled by control thread, we are still LOCKED, let us check
            //__________________________________________________________________
            if(pending.size>0)
            {
                //______________________________________________________________
                //
                // we take it !
                //______________________________________________________________
                task *todo = pending.pop_front();
                current.push_back(todo);

                //______________________________________________________________
                //
                // we tell the control thread that it can go further
                //______________________________________________________________
                if(verbose) { std::cerr << fn << "workers: signaling activity level-1 from " << ctx.size << "." << ctx.rank << std::endl; }
                activity.signal();
                access.unlock();

                //______________________________________________________________
                //
                // now this working thread is the only owner of the task
                // and access is granted for other threads to process
                //______________________________________________________________
                todo->proc(ctx);

                //______________________________________________________________
                //
                // we're done: remove task
                //______________________________________________________________
                access.lock();
                (void)current.unlink(todo);
                todo->~task();
                storage.store(todo);
            }

            // at this point, access must be LOCKED
            // we tell the control thread that it can go further
            if(verbose) { std::cerr << fn << "workers: signaling activity level-2 from " << ctx.size << "." << ctx.rank << std::endl; }
            activity.signal();
            goto WAITING_FOR_WORK;


        }

        job_id par_server:: enqueue( kernel &k )
        {

            //__________________________________________________________________
            //
            // take control
            //__________________________________________________________________
            YOCTO_LOCK(access);
            if(verbose) { std::cerr << "creating task #" << juuid << std::endl; }

            //__________________________________________________________________
            //
            // create the task and append it to the queue
            //__________________________________________________________________
            task *t = create_task(k);
            pending.push_back(t);


            //__________________________________________________________________
            //
            // wake up the control thread
            //__________________________________________________________________
            activity.signal();
            return t->uuid;
        }

        par_server::task * par_server:: create_task(const kernel &k)
        {
            
            task *t = (storage.size>0) ? storage.query() : object::acquire1<task>();
            try
            {
                new (t) task(juuid,k);
                ++juuid;
            }
            catch(...)
            {
                storage.store(t);
                throw;
            }
            return t;
        }


    }
}
