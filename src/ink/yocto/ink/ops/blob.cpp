
#include "yocto/ink/ops/blob.hpp"

namespace yocto
{
    namespace Ink
    {
        Blob:: ~Blob() throw()
        {
        }

        Blob:: Blob( const Bitmap &bmp ) :
        Pixmap<size_t>(bmp.w,bmp.h)
        {
        }

    }
}

