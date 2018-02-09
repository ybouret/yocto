
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
        
        RGBA indx2rgba:: get(const void *addr)
        {
            const size_t indx = *(const size_t *)addr;
            if(indx<=0)
            {
                return RGBA(0,0,0);
            }
            else
            {
                return NamedColor::FetchRGB(indx+shift);
            }
        }
    
        indx2rgba:: ~indx2rgba() throw() {}
        indx2rgba:: indx2rgba(const size_t sh) throw() : shift(sh) {}
        
    }
}
