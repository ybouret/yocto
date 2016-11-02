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
        ready(0)
        {
            init();
        }

#define __DISPLAY(CODE) do { if(verbose) { std::cerr << fn << CODE << std::endl; } } while(false)

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
            __DISPLAY("flushing...");
            if(pending.size>0)
            {
                // wait on a locked mutex
                //flushing.wait(access);

                // wake up on a locked mutex
                __DISPLAY("...all done");
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
                            __DISPLAY("synchronised");
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


        void par_server::loop(context &ctx) throw()
        {
            access.lock();
            __DISPLAY("enter " << ctx.size << "." << ctx.rank);
            ++ready;

            //__________________________________________________________________
            //
            // wait on a LOCKED mutex
            //__________________________________________________________________
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
                __DISPLAY("leave " << ctx.size << "." << ctx.rank);
                access.unlock();
                return;
            }

            


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
            // signal that there is some jobs to do
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
