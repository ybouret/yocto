#include "yocto/ink/ops/particles.hpp"


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


    }
}

