
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
            
            template <typename T>
            void apply( const Pixmap<T> &pxm, Engine &engine )
            {
                start(engine);
                source = &pxm;
                engine.submit(this, & Differential::gradThread<T>);
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
            
            template <typename T>
            void gradThread(const Domain &dom, threading::context &) throw()
            {
                assert(source);
                const Pixmap<T> &pxm = *(const Pixmap<T> *)source;
                
            }
        };
        
    }
}

#endif

