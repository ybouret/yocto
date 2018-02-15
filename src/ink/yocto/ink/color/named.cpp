
#include "yocto/ink/color/named.hpp"

namespace yocto
{
    namespace Ink
    {
        const NamedColor NamedColor::DB[] =
        {
#include "colors-db.inc"
        };
        
        const size_t NamedColor::DB_SIZE = sizeof(DB)/sizeof(DB[0]);
        
        RGB NamedColor:: FetchRGB(const size_t indx) throw()
        {
            const NamedColor &C = DB[indx%DB_SIZE];
            return RGB(C.r,C.g,C.b);
        }


        
    }
}
