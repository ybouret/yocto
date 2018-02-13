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

        Blur:: Blur(const float sig) :
        sigma( fabsf(sig)  ),
        sigm2( sigma*sigma ),
        scale( (sigm2>0.0f) ? 1.0f/(sigm2+sigm2) : 0.0f ),
        length( GetLength(sigma) ),
        wpatch( -length, length  ),
        weight( "weight", wpatch ),
        dpatch( coord(-length,-length), coord(length,length) ),
        dfield( "dfield", dpatch ),
        tgt(0),
        src(0)
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

