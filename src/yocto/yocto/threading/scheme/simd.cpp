#include "yocto/threading/scheme/simd.hpp"

namespace yocto
{
    namespace threading
    {
        _SIMD:: ~_SIMD() throw()
        {
        }

        _SIMD:: _SIMD( executor &xxx ) throw() :
        cpu(xxx)
        {
        }
        
    }
}

namespace yocto
{
    namespace threading
    {
        SIMD1:: ~SIMD1() throw()
        {
        }

        SIMD1:: SIMD1() throw() :
        seq_executor(),
        _SIMD(static_cast<executor&>(*this))
        {
        }

        void SIMD1:: run( kernel &k )
        {
            k(ctx);
        }
    }
}

namespace yocto
{
    namespace threading
    {
        static const char fn[] = "[SIMD] ";

        SIMD:: ~SIMD() throw()
        {
            quit();
        }

        SIMD:: SIMD(bool setVerbose) :
        par_executor(setVerbose),
        _SIMD(static_cast<executor&>(*this)),
        ready(0),
        cycle(),
        synch(),
        kproc(0),
        last_failure_index(0)
        {
            init();
        }


        void SIMD:: init()
        {
            try
            {
                //______________________________________________________________
                //
                // create threads
                //______________________________________________________________
                if(verbose)
                {
                    YOCTO_LOCK(access);
                    std::cerr << fn << "initializing " << size << " thread" << (size>1?"s":"") << std::endl;
                }

                for(size_t rank=0;rank<size;++rank)
                {
                    context &ctx = (*this)[rank];
                    ctx.priv     =   this;
                    workers.launch(call_loop,&ctx);
                }

                
                //______________________________________________________________
                //
                // waiting for first synchro
                //______________________________________________________________
                while(true)
                {
                    if( access.try_lock() )
                    {
                        if(ready>=size)
                        {
                            if(verbose) { std::cerr << fn << "all threads created" << std::endl; }
                            size_t iThread = 0;
                            for(thread *thr = workers.head; thr; thr=thr->next )
                            {
                                thr->on_cpu( cpu_index_of(iThread++) );
                            }
                            access.unlock();
                            if(verbose) { std::cerr << fn << "ready" << std::endl; }
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

        void SIMD:: quit() throw()
        {
            //__________________________________________________________________
            //
            // set stopping
            //__________________________________________________________________
            access.lock();
            stopping = true;
            if(verbose)
            {
                std::cerr << fn << "terminating..." << std::endl;
            }
            access.unlock();


            cycle.broadcast();

            //__________________________________________________________________
            //
            // waiting for threads to return
            //__________________________________________________________________
            workers.finish();
            if(verbose) { std::cerr << fn << "halted." << std::endl; }

        }



        void SIMD:: call_loop( void *args ) throw()
        {
            assert(args);
            context &ctx = *static_cast<context *>(args); assert(ctx.priv);
            static_cast<SIMD *>(ctx.priv)->loop(ctx);
        }


        void SIMD:: loop(context &ctx) throw()
        {

            //__________________________________________________________________
            //
            // first synchronization
            //__________________________________________________________________
            access.lock();
            if(verbose) { std::cerr << fn  << "synchronizing " << ctx.size << "." << ctx.rank << std::endl; }
            assert(ready<size);
            ++ready;


            //__________________________________________________________________
            //
            // waiting for instruction: wait on a LOCKED mutex
            //__________________________________________________________________
        WAIT_FOR_CYCLE:
            cycle.wait(access); // wait, access is unlocked for other threads

            //__________________________________________________________________
            //
            // here we woke up, access is LOCKED: check status
            //__________________________________________________________________
            if(stopping)
            {
                if(verbose) { YOCTO_LOCK(access); std::cerr << fn << "end of " << ctx.size << "." << ctx.rank << std::endl; }
                access.unlock();
                return; // end of all work
            }

            //__________________________________________________________________
            //
            // work to do
            //__________________________________________________________________
            assert(kproc);
            access.unlock();

            try
            {
                (*kproc)(ctx);
            }
            catch(...)
            {
                //TODO: send a message ?
                YOCTO_LOCK(access);
                last_failure_index = ctx.indx;
            }

            access.lock();
            ++ready;
            if(ready>=size)
            {
                synch.broadcast();
            }

            goto WAIT_FOR_CYCLE;

        }

        void SIMD:: run( kernel &k )
        {
            access.lock();         // lock access
            assert(size==ready);   // must be true here
            kproc = &k;            // local link
            ready = 0;             // global counter
            last_failure_index = 0;
            cycle.broadcast();     // would start all threads
            synch.wait(access);    // unlock access => start threads => come back LOCKED
            assert(size==ready);   // must be true here
            access.unlock();       // and unlock for next cycle...
            kproc = 0;
        }

    }
}
