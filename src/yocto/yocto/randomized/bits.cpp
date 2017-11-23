#include "yocto/randomized/bits.hpp"
#include <cstdlib>

namespace yocto
{
    namespace Randomized
    {
        Bits:: ~Bits() throw()
        {
        }

        Bits:: Bits(const uint32_t maxValue) throw() :
        span(maxValue),
        denD(double(span)+1.0),
        denF(float(span)+1.0f),
        halfDenD(denD/2),
        halfDenF(denF/2),
        addNumD(0.5-halfDenD),
        addNumF(0.5f-halfDenF)
        {
            
        }
    }
}

namespace yocto
{
    namespace Randomized
    {
        cstdbits::  cstdbits() throw() : yocto::Randomized::Bits(RAND_MAX) {}
        cstdbits:: ~cstdbits() throw() {}

        uint32_t cstdbits:: next32() throw() { return uint32_t(rand()); }
    }

}

