
#ifndef YOCTO_INK_OPS_DIFFERENTIAL_INCLUDED
#define YOCTO_INK_OPS_DIFFERENTIAL_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ink/pixmaps.hpp"
#include "yocto/math/types.hpp"

namespace yocto
{
    namespace Ink
    {

        class Gradient : public Area
        {
        public:
            typedef float        type;
            typedef Pixmap<type> pixmap;
            typedef pixmap::Row  row;
            static const size_t  BytesPerDomain = sizeof(type);

        protected:
            Pixmaps<type> fields;

        public:
            pixmap &gx;
            pixmap &gy;
            pixmap &gn;
            pixmap &ga;
            type    gn_max;

            explicit Gradient(const Bitmap &bmp);
            virtual ~Gradient() throw();

            template <typename T,typename FUNC>
            void compute(const Pixmap<T> &pxm,
                         FUNC            &func,
                         Engine          &engine,
                         bool             normalize=true)
            {
                std::cerr << "Computing with " << engine.size() << " max threads" << std::endl;
                source = &pxm;
                proc   = (void *) &func;
                engine.prepare(BytesPerDomain);
                engine.submit(this, & Gradient::computeThread<T,FUNC>);
                gn_max = 0;
                for(const Domain *dom = engine.head(); dom; dom=dom->next )
                {
                    const float *val  = (const float *)(dom->cache.data);
                    const float  vmax = val[0];
                    if(vmax>gn_max) gn_max = vmax;
                }
                if(normalize)
                {
                    if(gn_max>0)
                    {
                        engine.submit(this, &Gradient::normalizeThread<T>);
                    }
                }
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Gradient);
            const void *source;
            void       *proc;

            template <typename T,typename FUNC>
            void computeThread(const Domain &dom, threading::context &) throw()
            {
                assert(source);
                assert(proc);

                const Pixmap<T> &S    = *(const Pixmap<T> *)source;
                FUNC            &func = *(FUNC *)proc;

                const unit_t xmin = dom.x;
                const unit_t xmax = dom.x_end;

                const unit_t ymin = dom.y;
                const unit_t ymax = dom.y_end;

                type vmax=0;

                for(unit_t j=ymax;j>=ymin;--j)
                {
                    const unsigned                 jpos = this->ypos(j);
                    const typename Pixmap<T>::Row &Sj   = S[j];
                    row &gy_j = gy[j];
                    row &gx_j = gx[j];
                    row &gn_j = gn[j];
                    row &ga_j = ga[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        type           Gx  = 0;
                        type           Gy  = 0;

                        // X component
                        switch(xpos(i))
                        {
                            case AtLowerX: {
                                const type f0 = type(func(Sj[i]));
                                const type f1 = type(func(Sj[i+1]));
                                const type f2 = type(func(Sj[i+2]));
                                Gx = 4*f1-(3*f0+f2);
                            } break;
                            case AtUpperX: {
                                const type f0 = type(func(Sj[i]));
                                const type f1 = type(func(Sj[i-1]));
                                const type f2 = type(func(Sj[i-2]));
                                Gx = (3*f0+f2)-4*f1;
                            } break;
                            default:
                                assert(0==xpos(i));
                                Gx = type(func(Sj[i+1]))   - type(func(Sj[i-1]));
                                break;
                        }

                        // Y component
                        switch(jpos)
                        {
                            case AtLowerY: {
                                const type f0 = type(func(S[j][i]));
                                const type f1 = type(func(S[j+1][i]));
                                const type f2 = type(func(S[j+2][i]));
                                Gy = 4*f1-(3*f0+f2);
                            } break;
                            case AtUpperY : {
                                const type f0 = type(func(S[j][i]));
                                const type f1 = type(func(S[j-1][i]));
                                const type f2 = type(func(S[j-2][i]));
                                Gy = (3*f0+f2)-4*f1;
                            } break;
                            default:
                                assert(0==jpos);
                                Gy = type(func(S[j+1][i]))-type(func(S[j-1][i]));
                                break;
                        }

                        // storing component
                        gx_j[i] = Gx;
                        gy_j[i] = Gy;
                        const type nrm = math::Hypotenuse(Gx,Gy);
                        gn_j[i] = nrm;
                        if(nrm>vmax) vmax=nrm;
                        ga_j[i] = math::Atan2(Gx,Gy);
                    }
                }

                type *val = (type *)(dom.cache.data);
                val[0] = vmax;
            }

            template <typename T>
            void normalizeThread(const Domain &dom, threading::context &) throw()
            {
                const unit_t xmin = dom.x;
                const unit_t xmax = dom.x_end;

                const unit_t ymin = dom.y;
                const unit_t ymax = dom.y_end;
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    row &gn_j = gn[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        gn_j[i] /= gn_max;
                    }
                }
            }
        };

        
    }
}

#endif

