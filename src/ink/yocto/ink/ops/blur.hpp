#ifndef YOCTO_INK_OPS_BLUR_INCLUDED
#define YOCTO_INK_OPS_BLUR_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ink/pixmap.hpp"
#include "yocto/ink/color/rgb.hpp"
#include "yocto/ipso/field2d.hpp"

namespace yocto
{
    namespace Ink
    {

        class Blur
        {
        public:
            typedef ipso::field1D<float> Data1D;
            typedef ipso::field2D<float> Data2D;

            static unit_t GetLength(const float sig) throw();

            const float  sigma;
            const float  sigm2;
            const float  scale; //! 1.0f/(sig2+sig2)
            const unit_t length;
            explicit Blur(const float sig);
            explicit Blur(const float sig,const unit_t len);
            virtual ~Blur() throw();

            float operator()(const float dx) const throw();
            
            template <
            typename     VECTOR,
            typename     SCALAR,
            const size_t CHANNELS>
            inline void _apply(Pixmap<VECTOR>       &target,
                               const Pixmap<VECTOR> &source,
                               Engine               &engine)
            {
                tgt = &target;
                src = &source;
                engine.submit(this , & Blur::applyThreadX<VECTOR,SCALAR,CHANNELS> );
                engine.submit(this , & Blur::applyThreadY<VECTOR,SCALAR,CHANNELS> );
            }

            inline void apply(Pixmap<RGB>       &target,
                              const Pixmap<RGB> &source,
                              Engine            &engine)
            {
                _apply<RGB,uint8_t,3>(target,source,engine);
            }

            inline void apply(Pixmap<float>       &target,
                              const Pixmap<float> &source,
                              Engine              &engine)
            {
                _apply<float,float,1>(target,source,engine);
            }

            inline void apply(Pixmap<uint8_t>       &target,
                              const Pixmap<uint8_t> &source,
                              Engine                &engine)
            {
                _apply<uint8_t,uint8_t,1>(target,source,engine);
            }


        private:
            Data1D      weight;
            Data2D      dfield;
            void       *tgt;
            const void *src;
            void setup() throw();

            YOCTO_DISABLE_COPY_AND_ASSIGN(Blur);

            template<
            typename     VECTOR,
            typename     SCALAR,
            const size_t CHANNELS>
            void applyThreadX( const Domain &dom, threading::context & ) throw()
            {
                YOCTO_INK_AREA_LIMITS(dom);
                Pixmap<VECTOR>       &target = *(Pixmap<VECTOR>       *)tgt;
                const Pixmap<VECTOR> &source = *(const Pixmap<VECTOR> *)src;
                const unit_t          delta = length;
                const unit_t          x_end = target.x_end;

                for(unit_t i=xmax;i>=xmin;--i)
                {
                    const unit_t imin = max_of<unit_t>(0,i-delta);
                    const unit_t imax = min_of<unit_t>(x_end,i+delta);
                    const float  wmul = dfield[imin-i][imax-i];
                    for(unit_t j=ymax;j>=ymin;--j)
                    {
                        const typename Pixmap<VECTOR>::Row &S_j = source[j];
                        typename       Pixmap<VECTOR>::Row &T_j = target[j];

                        float num[4] = {0,0,0,0};
                        for(unit_t k=imax;k>=imin;--k)
                        {
                            const SCALAR *p  = (const SCALAR *)&S_j[k];
                            vecops<CHANNELS>::muladd(num,weight[k-i],p);
                        }
                        SCALAR *q = (SCALAR       *)&T_j[i];
                        for(size_t ch=0;ch<CHANNELS;++ch)
                        {
                            q[ch] = SCALAR(num[ch]*wmul);
                        }
                    }
                }
            }

            template<
            typename     VECTOR,
            typename     SCALAR,
            const size_t CHANNELS>
            void applyThreadY( const Domain &dom, threading::context & ) throw()
            {
                YOCTO_INK_AREA_LIMITS(dom);
                Pixmap<VECTOR>       &target = *(Pixmap<VECTOR>       *)tgt;
                const Pixmap<VECTOR> &source = *(const Pixmap<VECTOR> *)src;
                const unit_t delta = length;
                const unit_t y_end = target.y_end;

                for(unit_t j=ymax;j>=ymin;--j)
                {
                    const unit_t jmin = max_of<unit_t>(0,j-delta);
                    const unit_t jmax = min_of<unit_t>(y_end,j+delta);
                    const float  wmul = dfield[jmin-j][jmax-j];

                    typename Pixmap<VECTOR>::Row &T_j = target[j];

                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        float num[4] = {0,0,0,0};
                        for(unit_t k=jmax;k>=jmin;--k)
                        {
                            const SCALAR *p  = (const SCALAR *)&source[k][i];
                            vecops<CHANNELS>::muladd(num,weight[k-j],p);
                        }
                        
                        SCALAR *q = (SCALAR       *)&T_j[i];
                        for(size_t ch=0;ch<CHANNELS;++ch)
                        {
                            q[ch] = SCALAR(num[ch]*wmul);
                        }

                    }
                }
            }

        };
    }
}

#endif

