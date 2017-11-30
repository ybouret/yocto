#include "yocto/randomized/bits.hpp"
#include "yocto/code/binary.hpp"
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
        bits(bits_for(span)),
        half(span>>1),
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

#include "yocto/string.hpp"
#include "yocto/math/point3d.hpp"

namespace yocto
{
    namespace Randomized
    {
        template <>
        int Bits::get<int>()
        {
            return full<int>();
        }

        template <>
        float Bits::get<float>()
        {
            return to<float>();
        }

        template <>
        double Bits::get<double>()
        {
            return to<double>();
        }
        
        template <>
        string Bits::get<string>()
        {
            string ans;
            for(size_t i=3+leq(13);i>0;--i)
            {
                ans += 'a' + leq('z'-'a');
            }
            return ans;
        }

        template <>
        point2d<float> Bits::get< point2d<float> >()
        {
            return point2d<float>( to<float>(), to<float>() );
        }

        template <>
        point2d<double> Bits::get< point2d<double> >()
        {
            return point2d<double>( to<double>(), to<double>() );
        }

        template <>
        point3d<float> Bits::get< point3d<float> >()
        {
            return point3d<float>( to<float>(), to<float>(), to<float>() );
        }

        template <>
        point3d<double> Bits::get< point3d<double> >()
        {
            return point3d<double>( to<double>(), to<double>(), to<double>() );
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

        void cstdbits:: reseed(Bits &bits) throw()
        {
            srand( bits.full<unsigned>() );
        }

    }

}

