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

