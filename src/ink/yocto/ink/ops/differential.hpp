
#ifndef YOCTO_INK_OPS_DIFFERENTIAL_INCLUDED
#define YOCTO_INK_OPS_DIFFERENTIAL_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ink/pixmaps.hpp"

namespace yocto
{
    namespace Ink
    {
        
        class Differential : public PixmapF
        {
        public:
            //! uses only size
            explicit Differential(const Bitmap &bmp);
            virtual ~Differential() throw();
            
            template <typename T, typename FUNC>
            void apply(const Pixmap<T> &pxm,
                       FUNC            &func,
                       Engine          &engine)
            {
                start(engine);
                source = &pxm;
                proc   = (void *) &func;
                engine.submit(this, & Differential::gradThread<T,FUNC>);
            }
            
            void start( Engine &engine )
            {
                for(const Domain *dom=engine.head();dom;dom=dom->next)
                {
                    dom->cache.prepare(2*sizeof(type));
                }
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Differential);
            const void *source;
            void       *proc;

            template <typename T,typename FUNC>
            void gradThread(const Domain &dom, threading::context &) throw()
            {
                assert(source);
                assert(proc);
                assert(dom.cache.data);
                assert(dom.cache.size>=2*sizeof(type));
                const Pixmap<T> &pxm  = *(const Pixmap<T> *)source;
                FUNC            &func = *(FUNC *)proc;
                type             vmin = 0;
                type             vmax = 0;

                const unit_t xmin = dom.x;
                const unit_t xmax = dom.x_end;
                const unit_t xtop = x_end;

                const unit_t ymin = dom.y;
                const unit_t ymax = dom.y_end;
                const unit_t ytop = y_end;

                for(unit_t y=ymax;y>=ymin;--y)
                {
                    const bool at_ytop = (y>=ytop);
                    const bool at_ybot = (y<=0);
                    const typename Pixmap<T>::Row &S_y = pxm[y];
                    PixmapF::Row                  &V_y = (*this)[y];
                    for(unit_t x=xmax;x>=xmin;--x)
                    {
                        type gy = 0;
                        if(at_ybot)
                        {
                            
                        }
                        else
                        {

                        }
                        const bool at_xtop = (x>=xtop);
                        const bool at_ytop = (y>=ytop);
                    }
                }

                float *val = static_cast<float *>(dom.cache.data);
                val[0] = vmin;
                val[1] = vmax;
            }
        };
        
    }
}

#endif

