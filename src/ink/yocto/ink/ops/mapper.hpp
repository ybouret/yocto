#ifndef YOCTO_INK_OPS_MAPPER_INCLUDED
#define YOCTO_INK_OPS_MAPPER_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ink/pixmap.hpp"

namespace yocto
{
    namespace Ink
    {
        //! one to one pixel mapping functions
        class Mapper
        {
        public:
           

            template <typename T,typename U,typename FUNC> inline
            void operator()(Pixmap<T>       &tgt,
                            const Pixmap<U> &src,
                            FUNC            &func,
                            Engine          &engine)
            {
                target = &tgt;
                source = &src;
                proc   = (void*)&func;
                engine.submit(this, & Mapper::callThread<T,U,FUNC> );
            }

            template <
            typename T,
            typename U,
            typename V,
            typename FUNC> inline
            void operator()(Pixmap<T>       &tgt,
                            const Pixmap<U> &L,
                            const Pixmap<V> &R,
                            FUNC            &func,
                            Engine          &engine)
            {
                target = &tgt;
                lhs    = &L;
                rhs    = &R;
                proc   = (void *)&func;
                engine.submit(this, & Mapper::call2Thread<T,U,V,FUNC> );
            }



            Mapper() throw();
            virtual ~Mapper() throw();

            template <typename T,typename U,typename FUNC>
            void getMinMax( U &theMin, U &theMax, const Pixmap<T> &src, FUNC &pix2val, Engine &engine )
            {
                source = &src;
                proc   = (void *)&pix2val;
                // compute per domain
                engine.prepare(2*sizeof(U));
                engine.submit(this, & Mapper::getMinMaxThread<T,U,FUNC> );
                // reduction
                const Domain *dom = engine.head();
                const U      *val = static_cast<const U*>(dom->cache.data);
                theMin = val[0];
                theMax = val[1];
                for(dom=dom->next;dom;dom=dom->next)
                {
                    val = static_cast<const U*>(dom->cache.data);
                    theMin = min_of(val[0], theMin);
                    theMax = max_of(val[1], theMax);
                }
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Mapper);
            void       *target;
            const void *source;
            const void *lhs;
            const void *rhs;
            void       *proc;

            template <typename T,typename U,typename FUNC> inline
            void callThread( const Area &a, threading::context & ) throw()
            {
                YOCTO_INK_AREA_LIMITS(a);
                
                Pixmap<T>       &tgt  = *static_cast< Pixmap<T>       *>(target);
                const Pixmap<U> &src  = *static_cast< const Pixmap<U> *>(source);
                FUNC            &func = *(FUNC *)proc;
                
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    typename Pixmap<T>::Row       &Ty = tgt[j];
                    const typename Pixmap<U>::Row &Sy = src[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        Ty[i] = func(Sy[i]);
                    }
                }
            }

            template <typename T,typename U,typename V,typename FUNC> inline
            void call2Thread( const Area &a, threading::context & ) throw()
            {
                YOCTO_INK_AREA_LIMITS(a);
                assert(target);assert(lhs);assert(rhs);assert(proc);
                Pixmap<T>       &tgt  = *static_cast< Pixmap<T>       *>(target);
                const Pixmap<U> &L    = *static_cast< const Pixmap<U> *>(lhs);
                const Pixmap<V> &R    = *static_cast< const Pixmap<V> *>(rhs);
                FUNC            &func = *(FUNC *)proc;

                for(unit_t j=ymax;j>=ymin;--j)
                {
                    typename Pixmap<T>::Row       &Ty = tgt[j];
                    const typename Pixmap<U>::Row &Ly = L[j];
                    const typename Pixmap<V>::Row &Ry = R[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        Ty[i] = func(Ly[i],Ry[i]);
                    }
                }
            }

            template <typename T,typename U,typename FUNC>
            inline void getMinMaxThread( const Domain &dom, threading::context &) throw()
            {
                assert(dom.cache.data);
                assert(dom.cache.size>=2*sizeof(U));
                YOCTO_INK_AREA_LIMITS(dom);
                const Pixmap<T> &src = *static_cast< const Pixmap<T> *>(source);
                FUNC            &func = *(FUNC *)proc;
                U                vmin = func(src[ymax][xmax]);
                U                vmax = vmin;
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    const typename Pixmap<T>::Row &src_j= src[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        const U value = func(src_j[i]);
                        if(value<vmin) vmin = value;
                        if(value>vmax) vmax = value;
                    }
                }
                U *val = static_cast<U*>(dom.cache.data);
                val[0] = vmin;
                val[1] = vmax;
            }
        };
    }
}

#endif

