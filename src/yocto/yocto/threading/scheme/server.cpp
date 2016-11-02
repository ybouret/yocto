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

        void  seq_server:: enqueue_all(  array<kernel> &batch )
        {
            const size_t n = batch.size();
            for(size_t i=1;i<=n;++i)
            {
                batch[i](ctx);
                ++juuid;
            }
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
        ready(0)
        {
            init();
        }

#define __DISPLAY(CODE) do { if(verbose) { std::cerr << fn << CODE << std::endl; } } while(false)
#define __DISPLAY_SUB(CODE) __DISPLAY(ctx.size<<"."<<ctx.rank << ": " << CODE)

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
            __DISPLAY("stopping jobs");
            stopping = true;

            //__________________________________________________________________
            //
            // clean up pending tasks
            //__________________________________________________________________
            __DISPLAY("discarding #pending tasks: " << pending.size);
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
            __DISPLAY("ending threads");
            activity.broadcast();
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
            __DISPLAY("flushing request");
            if(pending.size>0||current.size>0)
            {
                // wait on a locked mutex
                flushing.wait(access);

                // wake up on a locked mutex
                __DISPLAY("flushing is done");
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
                    workers.launch(start,&ctx);
                }


                //______________________________________________________________
                //
                // wait for first synchro
                //______________________________________________________________
                while( true )
                {
                    if( access.try_lock() )
                    {
                        if(ready>=size)
                        {
                            __DISPLAY("threads are synchronised");
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


        void par_server:: start(void *args) throw()
        {
            assert(args);
            context &ctx = *static_cast<context *>(args); assert(ctx.priv);
            static_cast<par_server *>(ctx.priv)->loop(ctx);
        }



        //______________________________________________________________________
        //
        //
        // This is the main loop
        //
        //______________________________________________________________________


        void par_server::loop(context &ctx) throw()
        {
            access.lock();
            __DISPLAY_SUB("enter");
            ++ready;

            //__________________________________________________________________
            //
            // wait on a LOCKED mutex
            //__________________________________________________________________
        WAIT_FOR_ACTIVITY:
            activity.wait(access);

            //__________________________________________________________________
            //
            // wake up on a LOCKED mutex
            //__________________________________________________________________
            if(stopping)
            {
                //______________________________________________________________
                //
                // end of thread
                //______________________________________________________________
                __DISPLAY_SUB("leave");
                access.unlock();
                return;
            }

            //__________________________________________________________________
            //
            // ok, let us do something. At this point, we are LOCKED
            //__________________________________________________________________
            while(pending.size)
            {
                __DISPLAY_SUB("run task #" << pending.head->uuid);
                // take something to do
                task *t = pending.pop_front();
                current.push_back(t);

                // set activity for other
                if(pending.size)
                {
                    __DISPLAY_SUB("resignaling");
                    activity.signal();
                }

                // yield access
                access.unlock();

                // execute task
                t->proc(ctx);

                // take control
                access.lock();

                // remove task
                (void) current.unlink(t);
                t->~task();
                storage.store(t);

            }

            //__________________________________________________________________
            //
            // here, no more task, we are LOCKED
            //__________________________________________________________________
            __DISPLAY_SUB("no more pending tasks");
            if(current.size<=0)
            {
                __DISPLAY_SUB("all submitted tasks are done");
                flushing.signal();
            }

            goto WAIT_FOR_ACTIVITY;

        }

        job_id par_server:: enqueue( kernel &k )
        {

            //__________________________________________________________________
            //
            // take control
            //__________________________________________________________________
            YOCTO_LOCK(access);
            __DISPLAY("creating task #" << juuid);

            //__________________________________________________________________
            //
            // create the task and append it to the queue
            //__________________________________________________________________
            task *t = create_task(k);
            pending.push_back(t);


            //__________________________________________________________________
            //
            // signal that there is some jobs to do
            //__________________________________________________________________
            activity.signal();
            return t->uuid;
        }


        void par_server:: enqueue_all(  array<kernel> &batch )
        {
            //__________________________________________________________________
            //
            // take control
            //__________________________________________________________________
            YOCTO_LOCK(access);

            //__________________________________________________________________
            //
            // push back all tasks at once
            //__________________________________________________________________
            const size_t n = batch.size();
            for(size_t i=1;i<=n;++i)
            {
                pending.push_back( create_task(batch[i]) );
            }

            //__________________________________________________________________
            //
            // and go !
            //__________________________________________________________________

            activity.signal();
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
