
#include "yocto/ink/ops/blob.hpp"

namespace yocto
{
    namespace Ink
    {
        Blob:: ~Blob() throw()
        {
        }

        Blob:: Blob( const Bitmap &bmp ) :
        Pixmap<size_t>(bmp.w,bmp.h),
        tgt(0)
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

        void Blob:: topology(Particle       *inside,
                             Particle       *border,
                             const Particle &source,
                             const bool      connectFull) const
        {
            const Pixmap<size_t> &self = *this;
            const size_t kmax   = connectFull ? 8 : 4;
            if(inside)
            {
                (size_t &)(inside->indx) = source.indx;
            }
            if(border)
            {
                (size_t &)(border->indx) = source.indx;
            }
            for(const Vertex *vtx = source.head; vtx; vtx=vtx->next )
            {
                assert(source.indx==self[vtx->pos]);
                const coord p = vtx->pos;
                bool is_inside = true;
                for(size_t k=0;k<kmax;++k)
                {
                    const coord q = p + Core::Shift[k];
                    if(self.contains(q))
                    {
                        if(0==self[q])
                        {
                            is_inside = false;
                            break;
                        }
                    }
                }

                if(is_inside)
                {
                    if(inside)
                    {
                        inside->push_back( new Vertex(*vtx) );
                    }
                }
                else
                {
                    if(border)
                    {
                        border->push_back( new Vertex(*vtx) );
                    }
                }

            }
        }

        void Blob:: topology(Particles       *insides,
                             Particles       *borders,
                             const Particles &particles,
                             const bool       connectFull) const
        {
            if(insides)
            {
                insides->clear();
            }
            if(borders)
            {
                borders->clear();
            }
            for(const Particle *pt = particles.head;pt;pt=pt->next)
            {
                Particle *inside = NULL;
                Particle *border = NULL;
                if(insides)
                {
                    inside = new Particle(0);
                    insides->push_back( inside );
                }
                if(borders)
                {
                    border = new Particle(0);
                    borders->push_back(border);
                }
                topology(inside,border,*pt,connectFull);
            }
        }

        void Blob:: keepHoles(Particles &holes, const bool connectFull) throw()
        {
            const size_t kmax = connectFull ? 8 : 4;
            Particles    tmp;
            while(holes.size)
            {
                Particle    *hole      = holes.pop_back();
                bool         hole_flag = true;
                size_t       last_indx = 0;
                for(const Vertex *vtx=hole->head;vtx;vtx=vtx->next)
                {
                    assert(contains(vtx->pos));
                    const coord p = vtx->pos;
                    for(size_t k=0;k<kmax;++k)
                    {
                        const coord q = p + Core::Shift[k];
                        if(contains(q))
                        {
                            const size_t indx = (*this)[q];
                            if(indx==0)
                            {
                                // ok, nothing to do
                            }
                            else
                            {
                                if(0==last_indx)
                                {
                                    last_indx = indx;
                                }
                                else
                                {
                                    if(indx!=last_indx)
                                    {
                                        hole_flag = false;
                                        goto DONE;
                                    }
                                }
                            }
                        }
                    }
                }
            DONE:
                if(!hole_flag)
                {
                    delete hole;
                }
                else
                {
                    tmp.push_back(hole);
                }
            }
            holes.swap_with(tmp);
        }


    }
}

