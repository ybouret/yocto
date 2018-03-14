
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

        void Blob:: rewrite(Particles &particles,size_t indx)
        {
            Pixmap<size_t> &self = *this;
            if(indx<=0)
            {
                ldz();
            }
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

