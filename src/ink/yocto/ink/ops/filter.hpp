
#ifndef YOCTO_INK_OPS_FILTER_INCLUDED
#define YOCTO_INK_OPS_FILTER_INCLUDED 1

#include "yocto/ink/ops/local.hpp"

namespace yocto
{
    namespace Ink
    {

        //! run parallel local operations
        class Filter
        {
        public:
            explicit Filter() throw();
            virtual ~Filter() throw();

            template <typename T, typename FUNC>
            void run(Pixmap<T>       &target,
                     const Pixmap<T> &source,
                     FUNC            &func,
                     Engine          &engine)
            {
                static const size_t BytesPerDomain = memory::align(sizeof(Local)) + memory::align( Local::SIZE * sizeof(T) );
                engine.prepare(BytesPerDomain);
                tgt = &target;
                src = &source;
                pfn = (void *)&func;
                engine.submit(this, &Filter::runThread<T,FUNC>);
            }

          

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Filter);

            void       *tgt;
            const void *src;
            void       *pfn;

            template <typename T,typename FUNC>
            void runThread( const Domain &dom, threading::context &) throw()
            {
                static const size_t valueOffset = memory::align(sizeof(Local));

                assert(tgt); assert(src); assert(pfn); assert(dom.cache.data);
                Pixmap<T>        &target = *(Pixmap<T>*)tgt;
                const Pixmap<T>  &source = *(const Pixmap<T>*)src;
                FUNC             &func   = *(FUNC *)pfn;
                uint8_t          *wksp   = static_cast<uint8_t*>(dom.cache.data);
                Local            &agent  = *(Local *)&wksp[0];
                T                *value  =  (T     *)&wksp[valueOffset];

                YOCTO_INK_AREA_LIMITS(dom);
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    typename Pixmap<T>::Row &target_j = target[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        agent.collect(value,source,coord(i,j));
                        target_j[i] = func(value,agent.flags,agent.count);
                    }
                }
            }

        };

        //! a filter with internal memory
        template <typename T>
        class AutoFilter : public Filter, public Pixmap<T>
        {
        public:
            explicit AutoFilter(const Bitmap &bmp) : Filter(), Pixmap<T>(bmp.w,bmp.h)
            {
            }

            virtual ~AutoFilter() throw()
            {
            }

            template <typename FUNC>
            void transform( Pixmap<T> &source, FUNC &func, Engine &engine )
            {
                Pixmap<T> &target = *this;
                run(target,source,func,engine);
                source.copy(target);
            }
#define YOCTO_INK_AUTOFILTER(PROC) \
inline void PROC(Pixmap<T> &source, Engine &engine) { transform(source,Local::PROC<T>,engine); }

            YOCTO_INK_AUTOFILTER(Erode)
            YOCTO_INK_AUTOFILTER(Dilate)
            YOCTO_INK_AUTOFILTER(Average)
            YOCTO_INK_AUTOFILTER(Median)

            inline void Open(Pixmap<T> &source, Engine &engine)
            {
                Pixmap<T> &self = *this;
                run(self,source,Local::Erode<T>,engine);
                run(source,self,Local::Dilate<T>,engine);
            }

            inline void Close(Pixmap<T> &source, Engine &engine)
            {
                Pixmap<T> &self = *this;
                run(self,source,Local::Dilate<T>,engine);
                run(source,self,Local::Erode<T>,engine);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(AutoFilter);
        };
    }
}

#endif

