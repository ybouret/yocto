#include "yocto/seem/compiler.hpp"

namespace yocto
{
    namespace Seem
    {

        static const char seemCode[] =
        {
#include "seem.inc"
        };

        Compiler:: ~Compiler() throw() {}

        Compiler:: Compiler(const bool verbose) :
        Lang::Compiler( Lang::Syntax::Parser::CompileData("Seem Grammar", seemCode, sizeof(seemCode), verbose) )
        {
        }
        
    }
}
