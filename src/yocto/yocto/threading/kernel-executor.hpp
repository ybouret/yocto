#ifndef YOCTO_THREADING_KEXEC_INCLUDED
#define YOCTO_THREADING_KEXEC_INCLUDED 1

#include "yocto/threading/context.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/functor.hpp"

namespace yocto
{
    namespace threading
    {
        //! a threading kernel, using a context to know which data process
        typedef functor<void,TL1(context&)> kernel;

        //! interface to execute a kernel
        class kernel_executor : public counted_object
        {
        public:
            const size_t failure;

            virtual ~kernel_executor() throw();

            virtual void      operator()(kernel &) throw() = 0;
            virtual size_t    num_threads()  const throw() = 0;
            context &         operator[](const size_t rank) throw();
            const context &   operator[](const size_t rank) const throw();

            template <typename OBJECT_POINTER,typename METHOD_POINTER>
            inline void call( OBJECT_POINTER h, METHOD_POINTER m )
            {
                kernel_executor   &self = *this;
                kernel  k(h,m);
                self(k);
            }



        protected:
            explicit kernel_executor() throw();

            void no_failure() const throw();
            void set_failure(size_t) const throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(kernel_executor);
            virtual context *get_context(const size_t context_rank) const throw() = 0;
        };

        //! sequential executor, for debugging or single thread run
        class sequential_executor :
        public kernel_executor,
        public faked_lock,
        public context
        {
        public:
            virtual ~sequential_executor() throw();
            explicit sequential_executor() throw();
            virtual void  operator()(kernel &) throw();
            virtual size_t num_threads() const throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(sequential_executor);
            virtual context *get_context(const size_t context_rank) const throw();
        };

    }
}

#endif
