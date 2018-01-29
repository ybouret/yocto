#ifndef YOCTO_INK_OPS_CHANNELS_INCLUDED
#define YOCTO_INK_OPS_CHANNELS_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ink/pixmaps.hpp"

namespace yocto
{
    namespace Ink
    {
        class Channels
        {
        public:
            explicit Channels() throw() : source(0), target(0), cshift(0) {}
            virtual ~Channels() throw() {}

            inline void reset() throw() { source=0;target=0;cshift=0; }

            template <
            typename TYPE,
            typename UNIT>
            void split(const Pixmap<TYPE> &src,
                       Pixmaps<UNIT>      &tgt,
                       const    Domains   &doms,
                       threading::server  &srv,
                       const    size_t     shift=0)
            {
                std::cerr << "splitting " << tgt.size() << " channels, using " << srv.cpu.num_threads() << " threads" << std::endl;
                assert( (shift+tgt.size()) * sizeof(UNIT) <= sizeof(TYPE) );
                source = &src;
                target = &tgt;
                cshift = shift;
                doms.enqueue(srv, this, & Channels::splitThread<TYPE,UNIT> );
                srv.flush();
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Channels);
            const void *source;
            void       *target;
            size_t      cshift;

            template <
            typename TYPE,
            typename UNIT>
            void splitThread( const Rectangle &r, threading::context &ctx)
            {
                assert(source);
                assert(target);
                const    Pixmap<TYPE> &src   = *static_cast<const Pixmap<TYPE>*>(source);
                Pixmaps<UNIT>         &tgt   = *static_cast< Pixmaps<UNIT>    *>(target);
                const size_t           shift = 0;
                const unit_t xmin = r.x;
                const unit_t xmax = r.x_end;
                const unit_t ymin = r.y;
                const unit_t ymax = r.y_end;
                const size_t nch  = tgt.size();

                // YOCTO_LOCK(ctx.access);
                // std::cerr << "nch=" << nch << " rect: (" << r.x << "+" << r.w << "," << r.y << "+" << r.h << ")" << std::endl;


                for(unit_t y=ymax;y>=ymin;--y)
                {
                    const typename Pixmap<TYPE>::Row &Sy = src[y];
                    for(unit_t x=xmax;x>=xmin;--x)
                    {
                        const UNIT *Syx = (const UNIT *) &Sy[x];
                        Syx+=shift;
                        for(size_t ch=nch;ch>0;--ch)
                        {
                            (*(tgt[ch]))[y][x] = *(Syx++);
                        }
                    }
                }
            }
        };
    }
}

#endif

