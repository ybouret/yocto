
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
            static const size_t  BytesPerDomain = 2*sizeof(type);
        protected:
            Pixmaps<type> fields;

        public:
            pixmap &gx;
            pixmap &gy;
            pixmap &gn;

            explicit Gradient(const Bitmap &bmp);
            virtual ~Gradient() throw();

            template <typename T,typename FUNC>
            void compute(const Pixmap<T> &pxm,
                         FUNC            &func,
                         Engine          &engine)
            {
                std::cerr << "Computing with " << engine.size() << " max threads" << std::endl;
                source = &pxm;
                proc   = (void *) &func;
                engine.prepare(BytesPerDomain);
                engine.submit(this, & Gradient::computeThread<T,FUNC>);
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
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        const unsigned pos = jpos | this->xpos(i);
                        type           Gx  = 0;
                        type           Gy  = 0;
                        switch(pos)
                        {
                            case 0: // core
                                assert(i>0&&i<x_end);
                                assert(j>0&&j<y_end);
                                Gx = func(Sj[i+1])   - func(Sj[i-1]);
                                Gy = func(S[j+1][i]) - func(S[j-1][i]);
                                break;

                            default:
                                break;
                        }
                        gx_j[i] = Gx;
                        gy_j[i] = Gy;
                        const type nrm = math::Hypotenuse(Gx,Gy);
                        gn_j[i] = nrm;
                        if(nrm>vmax) vmax=nrm;
                    }
                }


            }
        };

        
    }
}

#endif

