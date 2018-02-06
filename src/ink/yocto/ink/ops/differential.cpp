
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
        }
        
    }
    
}


