
#include "yocto/ink/ops/differential.hpp"

namespace yocto
{
    namespace Ink
    {
        Gradient:: Gradient(const Bitmap &bmp) :
        Area(bmp),
        fields(3,bmp.w,bmp.h),
        gx( *fields[1] ),
        gy( *fields[2] ),
        gn( *fields[3] )
        {
            if(bmp.w<3||bmp.h<3) throw exception("Gradient: area is too small (%dx%d)", int(bmp.w), int(bmp.h));
        }

        Gradient:: ~Gradient() throw()
        {
        }
        

    }
    
}


