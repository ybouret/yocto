#include "yocto/ink/ops/blur.hpp"

namespace yocto
{
    namespace Ink
    {
        unit_t Blur::GetLength(const float sigma) throw()
        {
            const float dx = sqrtf( 16.0f * sigma*sigma * logf(2.0f) );
            return max_of<unit_t>(1,ceilf(dx));
        }

        Blur:: ~Blur() throw()
        {
            weight -= length;
            memory::kind<memory::global>::release_as(weight,wlen);
        }

        Blur:: Blur(const float sig) :
        sigma( fabsf(sig)  ),
        sigm2( sigma*sigma ),
        scale( (sigm2>0.0f) ? 1.0f/(sigm2+sigm2) : 0.0f ),
        length( GetLength(sigma) ),
        wlen( 2*length + 1 ),
        weight( memory::kind<memory::global>::acquire_as<float>(wlen) + length )
        {

        }



        float Blur:: operator()(const float dx) const throw()
        {
            return expf( - (dx*dx) * scale );
        }


    }
}

