#ifndef YOCTO_THREADING_SCHEME_SIMD_INCLUDED
#define YOCTO_THREADING_SCHEME_SIMD_INCLUDED 1

#include "yocto/threading/scheme/executor.hpp"

namespace yocto
{
    namespace threading
    {

        //! single instruction, multiple data interface
        class SIMD
        {
        public:
            executor &cpu;
            virtual  ~SIMD() throw();

            virtual void run( kernel &k ) = 0;

            template <typename HOST_POINTER, typename METHOD_POINTER>
            inline void load( HOST_POINTER host, METHOD_POINTER method )
            {
                kernel k(host,method);
                run(k);
            }


        protected:
            explicit SIMD(executor &xxx) throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(SIMD);
        };


        //! sequential code
        class SIMD1 : public seq_executor, public SIMD
        {
        public:
            explicit SIMD1() throw();
            virtual ~SIMD1() throw();

            virtual void run( kernel &k );
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(SIMD1);
        };

    }
}

#endif
