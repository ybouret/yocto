#ifndef YOCTO_LINGUA_XGEN_CPP_INCLUDED
#define YOCTO_LINGUA_XGEN_CPP_INCLUDED 1

#include "yocto/ios/istream.hpp"
#include "yocto/ios/ostream.hpp"
#include "yocto/lingua/token.hpp"

namespace yocto
{
    namespace lingua
    {

        class xgen_cpp
        {
        public:

            explicit xgen_cpp();
            virtual ~xgen_cpp() throw();

            void process( const string &filename, token &content );

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(xgen_cpp);
        };
    }
}

#endif
