#include "yocto/ink/ops/particles.hpp"
#include "yocto/sort/merge.hpp"

namespace yocto
{
    namespace Ink
    {

        Particle:: Particle(const size_t id) throw() :
        Vertex::List(),
        indx(id),
        next(0),
        prev(0)
        {
        }

        Particle:: ~Particle() throw()
        {
        }


        int Particle:: CompareBySize(const Particle *lhs, const Particle *rhs,void*) throw()
        {
            return __compare_decreasing(lhs->size,rhs->size);
        }

        point  Particle:: barycenter() const throw()
        {
            point G;
            if(size>0)
            {
                for(const Vertex *vtx = head;vtx;vtx=vtx->next)
                {
                    G.x += real_t(vtx->pos.x);
                    G.y += real_t(vtx->pos.y);
                }
                G/=real_t(size);
            }
            return G;
        }

        Area  Particle:: aabb() const throw()
        {
            if(size>0)
            {
                const Vertex *vtx = head;
                coord cmin = vtx->pos;
                coord cmax = cmin;
                for(vtx=vtx->next;vtx;vtx=vtx->next)
                {
                    const coord c = vtx->pos;
                    cmin.x = min_of(c.x,cmin.x);
                    cmin.y = min_of(c.y,cmax.y);

                    cmax.x = max_of(c.x,cmax.x);
                    cmax.y = max_of(c.y,cmax.y);
                }
                return Area(cmin.x,cmin.y,cmax.x-cmin.x+1,cmax.y-cmin.y+1);
            }
            else
            {
                return Area(0,0,0,0);
            }
        }
    }

}


namespace yocto
{
    namespace Ink
    {


        Particles:: Particles() throw()
        {
        }

        Particles:: ~Particles() throw()
        {
        }

        void Particles:: sortBySize()
        {
            Particle::List &self = *this;
            core::merging<Particle>::sort(self, Particle::CompareBySize, NULL);
        }

        void   Particles::  discardSmallerThan(const size_t count) throw()
        {
            Particle::List tmp;
            while(size>0)
            {
                Particle *pt = pop_front();
                if(pt->size<=count)
                {
                    delete pt;
                }
                else
                {
                    tmp.push_back(pt);
                }
            }

            size_t indx = 0;
            while(tmp.size>0)
            {
                Particle *pt = tmp.pop_front();
                (size_t &)(pt->indx) = ++indx;
                push_back(pt);
            }
        }


    }
}

