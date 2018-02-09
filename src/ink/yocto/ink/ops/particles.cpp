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

    }
}

