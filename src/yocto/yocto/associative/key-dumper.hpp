
//! \file key-dumper.hpp

#ifndef YOCTO_KEY_DUMPER_INCLUDED
#define YOCTO_KEY_DUMPER_INCLUDED 1


#include "yocto/hashing/function.hpp"

namespace yocto
{
    //! make a key from integral type
    template <
    typename T>
    class key_dumper
    {
    public:
        YOCTO_ARGUMENTS_DECL_T;

        inline  key_dumper() throw() {}
        inline ~key_dumper() throw() {}

        inline size_t operator()( param_type args ) throw()
        {
            size_t k = 0;
            hashing::function::fill(&k,sizeof(k), &args, sizeof(T) );
            return k;
        }
    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(key_dumper);
    };

}

#endif

