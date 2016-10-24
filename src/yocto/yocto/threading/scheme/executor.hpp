#ifndef YOCTO_THREADING_SCHEME_EXECUTOR
#define YOCTO_THREADING_SCHEME_EXECUTOR

#include "yocto/threading/context.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/threading/threads.hpp"
#include "yocto/threading/layout.hpp"

namespace yocto
{
    namespace threading
    {

        // executor: resources to execute some kernel(s)
        class executor : public counted_object
        {
        public:
            virtual ~executor() throw();

            virtual size_t    num_threads() const throw() = 0;

            context       & operator[](const size_t rank) throw();
            const context & operator[](const size_t rank) const throw();

        protected:
            explicit executor() throw();
            virtual context * get_context(const size_t rank) const throw() = 0;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(executor);

        };


        //! sequential executor
        class seq_executor : public executor
        {
        public:
            explicit seq_executor() throw();
            virtual ~seq_executor() throw();

            virtual size_t num_threads() const throw();

        protected:
            faked_lock        access;
            context           ctx;
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(seq_executor);
            virtual context * get_context(const size_t rank) const throw();
        };


        //! parallel executor
        class par_executor : public layout, public executor
        {
        public:
            explicit par_executor(bool setVerbose=false);
            virtual ~par_executor() throw();

            virtual size_t num_threads() const throw();

        protected:
            threads          workers;
        public:
            mutex            &access;
        protected:
            context_supply    contexts;
            bool              stopping; //!< flag to stop loops...

        private:
            virtual context * get_context(const size_t rank) const throw();
            YOCTO_DISABLE_COPY_AND_ASSIGN(par_executor);
        };

    }
}

#endif
