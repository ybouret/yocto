#ifndef YOCTO_THREADING_CREW_INCLUDED
#define YOCTO_THREADING_CREW_INCLUDED 1

#include "yocto/threading/kernel-executor.hpp"
#include "yocto/threading/layout.hpp"
#include "yocto/sequence/slots.hpp"
#include "yocto/threading/condition.hpp"
#include "yocto/functor.hpp"

namespace yocto
{
    namespace threading
    {

        //______________________________________________________________________
        //
        //! a crew of threads, will call kernel on each context
        //______________________________________________________________________
        class crew : public layout, public kernel_executor
        {
        public:
            explicit crew(bool setVerbose);
            explicit crew(const size_t num_cpus, const size_t cpu_start,bool setVerbose);
            virtual ~crew() throw();

            virtual void operator()(kernel &k) throw();
            virtual size_t num_threads() const throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(crew);
            threads           workers; //!< all the threads
        public:
            mutex            &access;  //!< shared access

        private:
            size_t            ready;    //!< for synchronyzation
            const bool        dying;    //!< flag for dying...
            kernel           *kproc;    //!< what to do for the current cycle
            condition         cycle;    //!< waiting for cycle
            condition         synch;    //!< waiting for synch
            slots_of<context> contexts;

            void init();
            void quit() throw();


            static void worker_call(void *) throw();
            void        worker_loop(context &ctx) throw();
            virtual context *get_context(const size_t context_rank) const throw();

        public:
            const size_t failure; //!< set to rank+1 if failure
        };
        
    };
}

#endif

