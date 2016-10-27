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
            access.lock();
            if(verbose) { std::cerr << fn << "stopping jobs" << std::endl; }
            stopping = true;

            // clean up
            if(verbose) { std::cerr << fn << "#pending tasks: " << pending.size << std::endl; }
            while(pending.size)
            {
                task *t = pending.pop_back();
                t->~task();
                object::release1<task>(t);
            }
            
            access.unlock();

            // wait for current tasks to end

            // finish/flush
            incoming.broadcast();
            activity.broadcast();

            // empty storage
            while(storage.size)
            {
                object::release1<task>( storage.query() );
            }

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


        //______________________________________________________________________
        //
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
            // wait on a LOCKED mutex
            activity.wait(access);

            // wake up on LOCKED mutex
            if(verbose) { std::cerr << fn << "control: activity detected" << std::endl; }
            if(stopping)
            {
                if(verbose) { std::cerr << fn << "control: leave" << std::endl; }
                access.unlock();
                return; // end of thread
            }
            if(verbose) { std::cerr << fn << "control: checking tasks" << std::endl; }

            if(pending.size)
            {
                incoming.signal();
            }



            // ok, end of it
            if(verbose) { std::cerr << fn << "control: waiting for next activity" << std::endl; }
            goto WAIT_FOR_ACTIVITY; // will unlock access

        }


        void par_server::workers_loop(context &ctx) throw()
        {
            access.lock();
            if(verbose) { std::cerr << fn << "workers: enter " << ctx.size << "." << ctx.rank << std::endl; }
            ++ready;

        WAITING_FOR_WORK:
            // wait on a LOCKED mutex
            incoming.wait(access);

            // wake up on a LOCKED mutex
            if(stopping)
            {
                if(verbose) { std::cerr << fn << "workers: leave " << ctx.size << "." << ctx.rank << std::endl; }
                access.unlock();
                return;
            }

            if(verbose) { std::cerr << fn << "workers: checking job for " << ctx.size << "." << ctx.rank << std::endl; }
            // ok, signaled by control thread, we are still LOCKED, let us check
            if(pending.size>0)
            {
                // we take it !
                task *todo = pending.pop_front();
                current.push_back(todo);

                // we ask the control thread that he can go further
                activity.signal();
                access.unlock();

                // now this working thread is the only owner of the task
                // and access is granted for other threads to process
                todo->proc(ctx);

                // we're done: remove task
                access.lock();
                (void)current.unlink(todo);
                todo->~task();
                storage.store(todo);
            }

            // at this point, access must be LOCKED
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
