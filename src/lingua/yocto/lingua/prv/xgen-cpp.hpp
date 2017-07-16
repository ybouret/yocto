#ifndef YOCTO_LINGUA_XGEN_CPP_INCLUDED
#define YOCTO_LINGUA_XGEN_CPP_INCLUDED 1


#include "yocto/lingua/lexer.hpp"
#include "yocto/hashing/mph.hpp"

namespace yocto
{
    namespace lingua
    {

        class xgen_cpp : public lexer
        {
        public:

            explicit xgen_cpp();
            virtual ~xgen_cpp() throw();

            void process(ios::istream &fp, l_list &lxm );

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(xgen_cpp);
            hashing::mperf hkw;
        };
    }
}

#endif
