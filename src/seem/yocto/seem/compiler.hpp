#ifndef YOCTO_SEEM_COMPILER_INCLUDED
#define YOCTO_SEEM_COMPILER_INCLUDED 1

#include "yocto/lang/compiler.hpp"

namespace yocto
{
    namespace Seem
    {
        class Compiler : public Lang::Compiler
        {
        public:
            explicit Compiler(const bool verbose=false);
            virtual ~Compiler() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Compiler);
        };
    }
}

#endif

