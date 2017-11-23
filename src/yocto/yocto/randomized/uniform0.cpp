#include "yocto/randomized/uniform0.hpp"

namespace yocto
{
    namespace Randomized
    {
        static const int32_t MASK  = 123459876;
        static const int32_t IA    = 16807;
        static const int32_t IM    = 2147483647; /* 0x7FFFFFFF = 2^31-1 */
        static const int32_t IQ    = 127773;
        static const int32_t IR    = 2836;

        Uniform0:: ~Uniform0() throw()
        {
        }

        Uniform0:: Uniform0() throw() :
        Bits(IM-1),
        seed_(0)
        {
        }

        uint32_t Uniform0:: next32() throw()
        {
            seed_ ^= MASK;
            const int32_t k = seed_ / IQ;
            seed_ = IA * ( seed_ - k * IQ ) - IR * k;
            if( seed_ < 0 )
                seed_ += IM;

            /* seed_ is in 0-(IM-1) */
            //const double res = ( 0.5 + (double)(seed_) )/( (double)IM );
            const uint32_t ans(seed_);
            seed_ ^= MASK;

            return ans;
        }

    }
}

