
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

        void Blob:: rewrite(Particles &particles)
        {
            Pixmap<size_t> &self = *this;
            ldz();
            size_t indx = 0;
            for( Particle *pt = particles.head; pt; pt=pt->next )
            {
                (size_t &)(pt->indx) = ++indx;
                for(const Vertex *vtx = pt->head;vtx;vtx=vtx->next)
                {
                    self[vtx->pos] = indx;
                }
            }
        }

    }
}

