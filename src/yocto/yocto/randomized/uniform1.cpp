#include "yocto/randomized/uniform1.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace Randomized
    {


        static const int32_t IA    = 16807;
        static const int32_t IM    = 2147483647; /* 0x7FFFFFFF = 2^31-1 */
        static const int32_t IQ    = 127773;
        static const int32_t IR    = 2836;
        static const int32_t NDIV  = (1+(IM-1)/32);

#if defined(_MSC_VER)
#pragma warning ( disable : 4351 )
#endif

        Uniform1:: ~Uniform1() throw()
        {
        }

        Uniform1:: Uniform1() throw() :
        Bits(IM-1),
        seed_(0 ),
        iy_( 0 ),
        iv_()
        {
            initialize(0);
        }


        void Uniform1:: reseed(Bits &bits) throw()
        {
            initialize( bits.full<int32_t>() );
        }

        void Uniform1:: initialize(const int32_t s) throw()
        {
            seed_ = s;
            if( seed_ <= 0 )
            {
                const int32_t ms = -seed_;
                seed_ = max_of<int32_t>(1, ms );
            }

            for( int32_t j = 32+7; j>0; --j )
            {
                const int32_t k = seed_ / IQ;
                seed_ = IA * ( seed_ - k * IQ ) - IR * k;
                if( seed_ < 0 )
                    seed_ += IM;
                if( j < 32 )
                    iv_[j] = seed_;
            }
            iy_ = iv_[0];
        }

        uint32_t Uniform1:: next32() throw()
        {
            const int32_t k = (seed_) / IQ;
            (seed_) = IA * ( (seed_) - k * IQ ) - IR * k;
            if( (seed_) < 0 )
                (seed_) += IM;

            const int32_t j = iy_ / NDIV;
            iy_    = iv_[j];
            iv_[j] = (seed_);
            /* iy is in 0;IM-1 */
            //return ( 0.5 + (double)(iy_) )/( (double)(IM) );
            return uint32_t(iy_);
        }
    }

}

