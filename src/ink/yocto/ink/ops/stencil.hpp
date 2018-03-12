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

            void display() const;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Stencil);
            void *       tgt;
            const void * src;
            float        weight;
            float        factor;
            bool         nrm;

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
                        if(nrm)
                        {
                            for(size_t i=0;i<CHANNELS;++i) { std::cerr << " " << wsum[i]; }
                            vecops<CHANNELS>::mul(wsum,factor);
                            std::cerr << " ->";for(size_t i=0;i<CHANNELS;++i) { std::cerr << " " << wsum[i]; } std::cerr << std::endl;
                        }
                        SCALAR *q = (SCALAR *)&target[j][i];
                        for(size_t ch=0;ch<CHANNELS;++ch)
                        {
                            q[ch] = Core::FloatToClosest<SCALAR>(wsum[ch]);
                        }
                    }
                }
            }
        };

        class Sobel3Y : public Stencil
        {
        public:
            inline explicit Sobel3Y() : Stencil("Sobel3Y",1,1)
            {
                StencilType &self = *this;
                self[1][-1]  =  1; self[1][0]   =  2; self[1][1]  =  1;
                self[-1][-1] = -1; self[-1][ 0] = -2; self[-1][1] = -1;
                compile();
            }

            inline virtual ~Sobel3Y() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Sobel3Y);
        };

        class Sobel3X : public Stencil
        {
        public:
            inline explicit Sobel3X() : Stencil("Sobel3X",1,1)
            {
                StencilType &self = *this;
                self[-1][-1] = -1; self[0][-1] = -2; self[1][-1] = -1;
                self[-1][1]  =  1; self[0][1]  =  2; self[1][1]  = 1;
                compile();
            }

            inline virtual ~Sobel3X() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Sobel3X);
        };


        class Scharr3Y : public Stencil
        {
        public:
            inline explicit Scharr3Y() : Stencil("Scharr3Y",1,1)
            {
                StencilType &self = *this;
                self[1][-1]  =  3; self[1][0]   =  10; self[1][1]  =  3;
                self[-1][-1] = -3; self[-1][ 0] = -10; self[-1][1] = -3;
                compile();
            }

            inline virtual ~Scharr3Y() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Scharr3Y);
        };

        class Scharr3X : public Stencil
        {
        public:
            inline explicit Scharr3X() : Stencil("Scharr3X",1,1)
            {
                StencilType &self = *this;
                self[-1][-1] = -3; self[0][-1] = -10; self[1][-1] = -3;
                self[-1][1]  =  3; self[0][1]  =  10; self[1][1]  = 3;
                compile();
            }

            inline virtual ~Scharr3X() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Scharr3X);
        };

    }
}

#endif

