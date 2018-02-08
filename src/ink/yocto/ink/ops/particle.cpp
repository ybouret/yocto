#include "yocto/ink/ops/particle.hpp"


namespace yocto
{
    namespace Ink
    {
        Particle:: ~Particle() throw() {}
        
        Particle::  Particle(const size_t id) throw() :
        indx(id),
        next(0),
        prev(0)
        {
        }
        
        
    }
}

namespace yocto
{
    namespace Ink
    {
        Particles:: ~Particles() throw(){}
        
        Particles:: Particles(const Bitmap &bmp) :
        ParticleMap(bmp.w,bmp.h),
        Particle::List()
        {
        }
        
    }
    
}
