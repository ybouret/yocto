#ifndef YOCTO_INK_FFT_INCLUDED
#define YOCTO_INK_FFT_INCLUDED 1

#include "yocto/ink/pixmaps.hpp"
#include "yocto/ink/parallel.hpp"

namespace yocto
{

    namespace Ink
    {

        class FFT
        {
        public:
            static unit_t LengthFor(const unit_t L);
            static Area   ComputeAreaFor(const Area &sub);
            explicit FFT() throw();
            virtual ~FFT() throw();

            template <typename T, typename FUNC>
            void load(PixmapZ         &zmap,
                      const Pixmap<T> &tsrc,
                      FUNC            &func,
                      Engine          &engine)
            {
                assert(zmap.contains(tsrc));
                zpx = &zmap;
                tpx = (void*)&tsrc;
                pfn = (void*)&func;
                engine.submit_no_flush(this, & FFT::loadThread<T,FUNC> );

                for(unit_t j=tsrc.y_end;j>=tsrc.y;--j)
                {
                    PixmapZ::Row &r_j = zmap[j];
                    for(unit_t i=zmap.x_end;i>tsrc.x_end;--i)
                    {
                        r_j[i] = 0;
                    }
                }
                for(unit_t j=zmap.y_end;j>tsrc.y_end;--j)
                {
                    memset( &zmap[j][0], 0, zmap.scanline );
                }
                engine.flush();
            }

            template <typename T, typename FUNC>
            void save(Pixmap<T>     &tsrc,
                      const PixmapZ &zmap,
                      FUNC          &func,
                      Engine        &engine)
            {
                assert(zmap.contains(tsrc));
                zpx = (PixmapZ *)&zmap;
                tpx = (void*)&tsrc;
                pfn = (void*)&func;
                engine.submit(this, & FFT::saveThread<T,FUNC> );
            }

            inline void forward(PixmapZ &zmap) throw() { apply(zmap,1); }
            inline void reverse(PixmapZ &zmap) throw() { apply(zmap,-1); }

        private:
            void apply(PixmapZ &zmap, const int isign) throw();
            YOCTO_DISABLE_COPY_AND_ASSIGN(FFT);
            PixmapZ *zpx;
            void    *pfn;
            void    *tpx;

            template <typename T,typename FUNC>
            void loadThread( const Domain &dom, threading::context &)
            {
                assert(zpx); assert(pfn); assert(tpx);
                PixmapZ           &zmap = *zpx;
                const   Pixmap<T> &tsrc = *(Pixmap<T> *)tpx;
                FUNC              &func = *(FUNC*)pfn;
                YOCTO_INK_AREA_LIMITS(dom);
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    PixmapZ::Row                  &z_j = zmap[j];
                    const typename Pixmap<T>::Row &t_j = tsrc[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        z_j[i] = func(t_j[i]);
                    }
                }
            }

            template <typename T,typename FUNC>
            void saveThread( const Domain &dom, threading::context &)
            {
                assert(zpx); assert(pfn); assert(tpx);
                const PixmapZ     &zmap = *zpx;
                Pixmap<T>         &tsrc = *(Pixmap<T> *)tpx;
                FUNC              &func = *(FUNC*)pfn;
                YOCTO_INK_AREA_LIMITS(dom);
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    const PixmapZ::Row            &z_j = zmap[j];
                    typename Pixmap<T>::Row       &t_j = tsrc[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        t_j[i] = func(z_j[i]);
                    }
                }
            }
        };

    }

}

#endif

