#ifndef YOCTO_THREADING_SCHEME_SIMD_INCLUDED
#define YOCTO_THREADING_SCHEME_SIMD_INCLUDED 1

#include "yocto/threading/scheme/executor.hpp"
#include "yocto/threading/condition.hpp"

namespace yocto
{
    namespace threading
    {

        //! single instruction, multiple data interface
        /**
         the same kernel will be called on a different context
         */
        class _SIMD
        {
        public:
            executor &cpu;
            virtual  ~_SIMD() throw();

            virtual void run( kernel &k ) = 0;

            template <typename HOST_POINTER, typename METHOD_POINTER>
            inline void load( HOST_POINTER host, METHOD_POINTER method )
            {
                kernel k(host,method);
                run(k);
            }


        protected:
            explicit _SIMD(executor &xxx) throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(_SIMD);
        };


        //______________________________________________________________________
        //
        //! sequential code
        //______________________________________________________________________
        class SIMD1 : public seq_executor, public _SIMD
        {
        public:
            explicit SIMD1() throw();
            virtual ~SIMD1() throw();

            virtual void run( kernel &k );
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(SIMD1);
        };


        //______________________________________________________________________
        //
        //! parallel code
        //______________________________________________________________________
        class SIMD : public par_executor, public _SIMD
        {
        public:
            explicit SIMD(bool setVerbose=false);
            virtual ~SIMD() throw();

            virtual void run( kernel &k );


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(SIMD);
            size_t    ready; //! for synchronisation
            condition cycle; //!< waiting for cycle
            condition synch; //!< waiting for synch
            kernel   *kproc; //!< local work
            
            void init();
            void quit() throw();

            static void call_loop( void *args ) throw(); // thread entry point
            void loop(context &) throw();

        public:
            size_t last_failure_index; //! index of thread in case of failure
            
        };

    }
}

#endif
