#ifndef YOCTO_INK_OPS_STENCIL_INCLUDED
#define YOCTO_INK_OPS_STENCIL_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ipso/field2d.hpp"
#include "yocto/ink/pixmap.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef ipso::field2D<float> StencilType;

        class Stencil : public StencilType
        {
        public:
            const unit_t dx;
            const unit_t dy;
            //! ctor : (2*w+1)x(2*h+1)
            explicit Stencil(const string &id, const unit_t w, const unit_t h) :
            StencilType(id,coord(-w,-h),coord(w,h)),
            dx(upper.x),
            dy(upper.y)
            {
            }

            explicit Stencil(const char *id, const unit_t w, const unit_t h) :
            StencilType(id,coord(-w,-h),coord(w,h)),
            dx(upper.x),
            dy(upper.y)
            {
            }

            virtual ~Stencil() throw()
            {
            }

            template <
            typename     VECTOR,
            typename     SCALAR,
            const size_t CHANNELS>
            inline void _apply(Pixmap<VECTOR>       &target,
                               const Pixmap<VECTOR> &source,
                               Engine               &engine)
            {
                std::cerr << "Apply stencil '" << this->name << "' " << *this << std::endl;
                tgt = &target;
                src = &source;
                engine.submit(this, &Stencil::applyThread<VECTOR,SCALAR,CHANNELS> );
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Stencil);
            void *       tgt;
            const void * src;

            template <
            typename     VECTOR,
            typename     SCALAR,
            const size_t CHANNELS>
            inline void applyThread( const Area &dom, threading::context & )
            {
                assert(tgt); assert(src);
                Pixmap<VECTOR>       &target = *static_cast< Pixmap<VECTOR> *       >(tgt);
                const Pixmap<VECTOR> &source = *static_cast< const Pixmap<VECTOR> * >(src);

                YOCTO_INK_AREA_LIMITS(dom);
                const unit_t       x_end = target.x_end;
                const unit_t       y_end = target.y_end;
                const StencilType &self  = *this;
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    const unit_t jlo = max_of<unit_t>(0,j-dy);
                    const unit_t jhi = min_of<unit_t>(y_end,j+dy);
                    const unit_t ylo = jlo-j;
                    const unit_t yhi = jhi-j;

                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        const unit_t ilo = max_of<unit_t>(0,i-dx);
                        const unit_t ihi = min_of<unit_t>(x_end,i+dx);
                        const unit_t xlo = ilo-i;
                        const unit_t xhi = ihi-i;

                        float csum = 0.0f;
                        float wsum[4] = { 0,0,0,0 };
                        for(unit_t yy=yhi;yy>=ylo;--yy)
                        {
                            const StencilType::row &sten_y = self[yy];
                            for(unit_t xx=xhi;xx>=xlo;--xx)
                            {
                                const float coef = sten_y[xx];
                                csum += coef;
                                const SCALAR *p = (const SCALAR *)&source[j+yy][i+xx];
                                vecops<CHANNELS>::muladd(wsum,coef,p);
                            }
                        }

                        SCALAR *q = (SCALAR *) &target[j][i];
                        for(size_t ch=0;ch<CHANNELS;++ch)
                        {
                            q[ch] = wsum[ch];
                        }
                    }

                }

            }
        };
    }
}

#endif

