#include "yocto/program.hpp"
#include "yocto/lang/syntax/parser.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_PROGRAM_START()
{
    if(argc<=1)
    {
        throw exception("usage: %s grammar.g",program);
    }
    
}
YOCTO_PROGRAM_END()
