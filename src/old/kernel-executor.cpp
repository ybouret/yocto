#include "yocto/threading/kernel-executor.hpp"

namespace yocto
{
    namespace threading
    {
        kernel_executor::  kernel_executor() throw() : failure(0) {}
        kernel_executor:: ~kernel_executor() throw() {}

        void kernel_executor:: no_failure() const throw() { (size_t&)failure = 0; }
        void kernel_executor:: set_failure(const size_t f) const throw() { (size_t&)failure = f; }


        context & kernel_executor:: operator[](const size_t rank) throw()
        {
            return * get_context(rank);
        }

        const context & kernel_executor:: operator[](const size_t rank) const throw()
        {
            return * get_context(rank);
        }


    }

}

namespace yocto
{
    namespace threading
    {


        sequential_executor:: ~sequential_executor() throw() {}

        sequential_executor:: sequential_executor() throw() :
        kernel_executor(),
        faked_lock(),
        context(0,1,*this)
        {
        }


        void sequential_executor:: operator()(kernel &k) throw()
        {
            no_failure();
            try
            {
                k(*this);
            }
            catch(...)
            {
                set_failure(1);
            }
        }

        size_t sequential_executor:: num_threads() const throw() { return 1; }

        context * sequential_executor:: get_context(const size_t context_rank) const throw()
        {
            assert(context_rank==0);
            const context &ctx = *this;
            return (context *)&ctx;
        }
    }
    
}

