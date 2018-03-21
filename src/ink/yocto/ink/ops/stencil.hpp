#ifndef YOCTO_INK_OPS_STENCIL_INCLUDED
#define YOCTO_INK_OPS_STENCIL_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ipso/field2d.hpp"
#include "yocto/ink/pixmaps.hpp"
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
            explicit Stencil(const string &id, const unit_t w, const unit_t h);
            explicit Stencil(const char   *id, const unit_t w, const unit_t h);
            virtual ~Stencil() throw();
            void     compile() throw();
            
            template <
            typename     VECTOR,
            typename     SCALAR,
            const size_t CHANNELS>
            inline void _apply(Pixmap<VECTOR>       &target,
                               const Pixmap<VECTOR> &source,
                               Engine               &engine,
                               const bool            normalize)
            {
                //std::cerr << "Apply stencil '" << this->name << "' " << *this << std::endl;
                tgt = &target;
                src = &source;
                nrm = normalize;
                if(nrm)
                {
                    vmin = -negative_weight * float( Pixel<SCALAR>::Opaque );
                    vmax =  positive_weight * float( Pixel<SCALAR>::Opaque );
                    //std::cerr << "vmin=" << vmin << ", vmax=" << vmax << std::endl;
                }
                engine.submit(this, &Stencil::applyRawThread<VECTOR,SCALAR,CHANNELS> );
            }
            
            inline void apply(Pixmap<float>       &target,
                              const Pixmap<float> &source,
                              Engine              &engine,
                              const bool           normalize)
            {
                _apply<float,float,1>(target,source,engine,normalize);
            }
            
            inline void apply(Pixmap<uint8_t>       &target,
                              const Pixmap<uint8_t> &source,
                              Engine                &engine,
                              const bool             normalize)
            {
                _apply<uint8_t,uint8_t,1>(target,source,engine,normalize);
            }
            
            inline void apply(Pixmap<RGB>       &target,
                              const Pixmap<RGB> &source,
                              Engine            &engine,
                              const bool         normalize)
            {
                _apply<RGB,uint8_t,3>(target,source,engine,normalize);
            }
            
            //! display as a matrix
            void display() const;
            
            //! print as an array
            void print() const;
            
            inline float compute(const Pixmap<float> &pxm, const unit_t x, const unit_t y) const throw()
            {
                float sum = 0.0f;
                for(unit_t yy=upper.y;yy>=lower.y;--yy)
                {
                    const ipso::field1D<float> &self_y = (*this)[yy];
                    const unit_t                j      = pxm.zfy(yy+y);
                    const Pixmap<float>::Row   &pxm_j  = pxm[j];
                    for(unit_t xx=upper.x;xx>=lower.x;--xx)
                    {
                        const float   w = self_y[xx];
                        const unit_t  i = pxm.zfx(xx+x);
                        sum += w * pxm_j[i];
                    }
                }
                return sum;
            }
            
            static inline
            void compute(float               &gx,
                         const Stencil       &sx,
                         float               &gy,
                         const Stencil       &sy,
                         const Pixmap<float> &pxm,
                         const unit_t         x,
                         const unit_t         y)
            {
                assert( ipso::patch2D::eq(sx,sy) );
                float sum_x = 0.0f, sum_y=0.0f;
                const unit_t xmin = sx.lower.x;
                const unit_t xmax = sy.upper.x;
                const unit_t ymin = sx.lower.y;
                const unit_t ymax = sx.upper.y;
                for(unit_t yy=ymax;yy>=ymin;--yy)
                {
                    const ipso::field1D<float> &sx_yy = sx[yy];
                    const ipso::field1D<float> &sy_yy = sy[yy];
                    const unit_t j = pxm.zfy(y+yy);
                    const Pixmap<float>::Row &pxm_j = pxm[j];
                    for(unit_t xx=xmax;xx>=xmin;--xx)
                    {
                        const float  wx    = sx_yy[xx];
                        const float  wy    = sy_yy[xx];
                        const unit_t i     = pxm.zfx(x+xx);
                        const float  value = pxm_j[i];
                        
                        sum_x += wx * value;
                        sum_y += wy * value;
                    }
                }
                gx = sum_x;
                gy = sum_y;
            }
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Stencil);
            void *       tgt;
            const void * src;
            float        negative_weight;
            float        positive_weight;
            float        weight; //!< positive+negative
            float        factor; //!< 1.0/weight
            bool         nrm;    //!< if normalize
            float        vmin;   //!< minimal value for this stencil
            float        vmax;   //!< maximal value for this stencil
            
            template <
            typename     VECTOR,
            typename     SCALAR,
            const size_t CHANNELS>
            inline void applyRawThread( const Area &dom, threading::context & )
            {
                assert(tgt); assert(src);
                Pixmap<VECTOR>       &target = *static_cast< Pixmap<VECTOR> *       >(tgt);
                const Pixmap<VECTOR> &source = *static_cast< const Pixmap<VECTOR> * >(src);
                const StencilType    &self   = *this;
                YOCTO_INK_AREA_LIMITS(dom);
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        float wsum[4] = { 0,0,0,0 };
                        // accumulation
                        for(unit_t yy=upper.y;yy>=lower.y;--yy)
                        {
                            const unit_t y = source.zfy(j+yy);
                            for(unit_t xx=upper.x;xx>=lower.x;--xx)
                            {
                                const float   w = self[yy][xx];
                                const unit_t  x = source.zfx(i+xx);
                                const SCALAR *p = (const SCALAR *)&source[y][x];
                                vecops<CHANNELS>::muladd(wsum,w,p);
                            }
                        }
                        // end accumulation

                        // normalize?
                        if(nrm)
                        {
                            for(size_t i=0;i<CHANNELS;++i)
                            {
                                wsum[i] = (wsum[i]-vmin)*factor;
                            }
                        }

                        // store values
                        SCALAR *q = (SCALAR *)&target[j][i];
                        for(size_t ch=0;ch<CHANNELS;++ch)
                        {
                            q[ch] = Core::FloatToClosest<SCALAR>(wsum[ch]);
                        }
                    }
                }
            }
        };
        
        class StencilDX : public Stencil
        {
        public:
            inline explicit StencilDX() : Stencil("StencilDX",1,1)
            {
                Stencil &self = *this;
                self[0][-1] = -1; self[0][1] = 1;
                compile();
            }
            
            inline virtual ~StencilDX() throw()
            {
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(StencilDX);
        };
        
        class StencilDY : public Stencil
        {
        public:
            inline explicit StencilDY() : Stencil("StencilDY",1,1)
            {
                Stencil &self = *this;
                self[-1][0] = -1; self[1][0] = 1;
                compile();
            }
            
            inline virtual ~StencilDY() throw()
            {
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(StencilDY);
        };
        
        
        
        
        
    }
}

#endif

