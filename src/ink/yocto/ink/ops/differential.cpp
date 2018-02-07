
#include "yocto/ink/ops/differential.hpp"

namespace yocto
{
    namespace Ink
    {
        
        Differential:: ~Differential() throw()
        {
        }
        
        Differential:: Differential(const Bitmap &bmp) :
        PixmapF(bmp.w,bmp.h)
        {
            if(bmp.w<3||bmp.h<3) throw exception("Differential: area is too small (%dx%d)", int(bmp.w), int(bmp.h));
        }
        
    }
    
}


