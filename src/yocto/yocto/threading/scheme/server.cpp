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
        storage()
        {
        }


        void par_server:: quit() throw()
        {
            access.lock();
            stopping = true;
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


        void par_server::control_loop() throw()
        {
        }


        void par_server::workers_loop(context &ctx) throw()
        {

        }

    }
}
