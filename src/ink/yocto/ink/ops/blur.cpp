#include "yocto/ink/ops/blur.hpp"

namespace yocto
{
    namespace Ink
    {
        unit_t Blur::GetLength(const float sigma) throw()
        {
            const float dx = sqrtf( 2.0f * sigma*sigma * logf(256.0f) );
            return max_of<unit_t>(1,ceilf(dx));
        }

        Blur:: ~Blur() throw()
        {

        }
#define Y_BLUR_TWO_PI math::numeric<float>::two_pi

#define Y_BLUR_CTOR(LEN)                                          \
sigma( fabsf(sig)  ),                                             \
sigm2( sigma*sigma ),                                             \
scale( (sigm2>0.0f) ? 1.0f/(sigm2+sigm2) : 0.0f ),                \
length( LEN ),                                                    \
width(1+2*length),                                                \
weight( "weight", -length, length ),                              \
dfield( "dfield", coord(-length,-length), coord(length,length) ), \
tgt(0),                                                           \
src(0)

        Blur:: Blur(const float sig) :
        Y_BLUR_CTOR( GetLength(sigma) )
        {
            setup();
        }

        Blur:: Blur(const float sig, const unit_t len) :
        Y_BLUR_CTOR( abs_of(len) )
        {
            setup();
        }

        void Blur:: setup() throw()
        {
            Blur &self = *this;
            weight[0]=1.0f;
            for(unit_t dx=1;dx<=length;++dx)
            {
                weight[dx] = weight[-dx] = self(dx);
            }
            for(unit_t ini=-length;ini<=length;++ini)
            {
                for(unit_t fin=ini;fin<=length;++fin)
                {
                    float sum = 0;
                    for(unit_t k=ini;k<=fin;++k)
                    {
                        sum += weight[k];
                    }
                    dfield[ini][fin] = 1.0f/sum;
                }
            }
        }


        float Blur:: operator()(const float dx) const throw()
        {
            return expf( - (dx*dx) * scale );
        }


    }
}

