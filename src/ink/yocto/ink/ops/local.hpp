#ifndef YOCTO_INK_OPS_LOCAL_INCLUDED
#define YOCTO_INK_OPS_LOCAL_INCLUDED 1

#include "yocto/ink/pixmap.hpp"
#include "yocto/ink/color/pixel.hpp"
#include "yocto/ink/parallel.hpp"
#include "yocto/code/bmove.hpp"

namespace yocto
{
    namespace Ink
    {


        class Local
        {
        public:
            static const size_t Left   = 0x01;
            static const size_t Right  = 0x02;
            static const size_t Bottom = 0x04;
            static const size_t Top    = 0x08;
            static const size_t SIZE   = 9;
            static const coord  Position[SIZE];
            static const size_t Flags[SIZE];
            static  size_t      GetPositionIndex(const size_t flag);

            virtual ~Local() throw();
            explicit Local() throw();


            size_t     count;         //!< extracted count
            size_t     flags[SIZE];   //!< extracted size

            template <typename T>
            inline void collect(T               *value,
                                const Pixmap<T> &source,
                                const coord      p0 ) throw()
            {
                count = 0;
                for(size_t k=0;k<SIZE;++k)
                {
                    const coord p = p0 + Position[k];
                    if(source.has(p))
                    {
                        bmove(value[count],source[p]);
                        flags[count] = Flags[k];
                        ++count;
                    }
                }
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Local);
        };

        class Filter
        {
        public:
            explicit Filter() : tgt(0), src(0), pfn(0) {}
            virtual ~Filter() throw() {}

            template <typename T, typename FUNC>
            void run( Pixmap<T> &target, const Pixmap<T> &source, FUNC &func, Engine &engine )
            {
                static const size_t BytesPerDomain = memory::align(sizeof(Local)) + memory::align( Local::SIZE * sizeof(T) );
                engine.prepare(BytesPerDomain);
                tgt = &target;
                src = &source;
                pfn = (void *)&func;
                engine.submit(this, &Filter::runThread<T,FUNC>);
            }

            template <typename T> static inline
            T Erode( const T *value, const size_t *flags, const size_t count ) throw()
            {
                assert(count>0);
                T ans = value[0];
                for(size_t k=1;k<count;++k)
                {
                    ans = min_of(ans,value[k]);
                }
                return ans;
            }

            template <typename T> static inline
            T Dilate( const T *value, const size_t *flags, const size_t count ) throw()
            {
                assert(count>0);
                T ans = value[0];
                for(size_t k=1;k<count;++k)
                {
                    ans = max_of(ans,value[k]);
                }
                return ans;
            }

            template <typename T> static inline
            T Average( const T *value, const size_t *flags, const size_t count ) throw()
            {
                assert(count>0);
                T ans = value[0];
                for(size_t k=1;k<count;++k)
                {
                    ans = Pixel<T>::Average(value,count);
                }
                return ans;
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
            void operator()( Pixmap<T> &source, FUNC &func, Engine &engine )
            {
                Pixmap<T> &target = *this;
                run(target,source,func,engine);
                source.copy(target);
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(AutoFilter);
        };

    }
}

#endif

