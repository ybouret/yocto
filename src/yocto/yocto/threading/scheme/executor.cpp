#include "yocto/threading/scheme/executor.hpp"

namespace yocto
{
    namespace threading
    {

        executor:: ~executor() throw() {}
        executor::  executor() throw() {}

        context & executor:: operator[](const size_t rank) throw()
        {
            return *get_context(rank);
        }

        const context & executor:: operator[](const size_t rank) const throw()
        {
            return *get_context(rank);
        }


    }
}


namespace yocto
{
    namespace threading
    {
        
        seq_executor:: seq_executor() throw() : access(), ctx(0,1,access)
        {
        }

        seq_executor:: ~seq_executor() throw()
        {
        }

        size_t seq_executor:: num_threads() const throw()
        {
            return 1;
        }

        context * seq_executor:: get_context(const size_t rank) const throw()
        {
            assert(0==rank);
            return (context *)&ctx;
        }

    }
}


namespace yocto
{
    namespace threading
    {
        par_executor:: par_executor(bool setVerbose) :
        yocto::threading::layout(setVerbose),
        workers("executor",size),
        access(workers.access),
        contexts(size,access),
        stopping(false)
        {
        }

        par_executor:: ~par_executor() throw()
        {
        }

        context * par_executor:: get_context(const size_t rank) const throw()
        {
            return (context *)&contexts[rank];
        }

        size_t par_executor:: num_threads() const throw()
        {
            assert(size==workers.size);
            return size;
        }
    }
}


