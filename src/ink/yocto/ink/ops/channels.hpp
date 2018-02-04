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
            explicit Channels() throw();
            virtual ~Channels() throw();
            
            template <
            typename TYPE,
            typename UNIT>
            void split(const Pixmap<TYPE> &src,
                       Pixmaps<UNIT>      &tgt,
                       const    Domains   &doms,
                       const    size_t     shift=0)
            {
                std::cerr << "split " << tgt.size() << " channels, using " << doms.srv->cpu.num_threads() << " threads/#dom=" << doms.size << std::endl;
                assert( (shift+tgt.size()) * sizeof(UNIT) <= sizeof(TYPE) );
                source = &src;
                target = &tgt;
                cshift = shift;
                doms.submit(this, & Channels::splitThread<TYPE,UNIT> );
            }

            template <
            typename TYPE,
            typename UNIT>
            void merge(Pixmap<TYPE>        &tgt,
                       const Pixmaps<UNIT> &src,
                       const    Domains   &doms,
                       const    size_t     shift=0)
            {
                std::cerr << "merge " << src.size() << " channels, using " << doms.srv->cpu.num_threads() << " threads/#dom=" << doms.size << std::endl;
                assert( (shift+src.size()) * sizeof(UNIT) <= sizeof(TYPE) );
                source = &src;
                target = &tgt;
                cshift = shift;
                doms.submit(this, & Channels::mergeThread<TYPE,UNIT> );
            }



        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Channels);
            const void *source;
            void       *target;
            size_t      cshift;

            template <
            typename TYPE,
            typename UNIT>
            void splitThread( const Area &r, threading::context &) throw()
            {
                assert(source);
                assert(target);
                const    Pixmap<TYPE> &src   = *static_cast<const Pixmap<TYPE>*>(source);
                Pixmaps<UNIT>         &tgt   = *static_cast< Pixmaps<UNIT>    *>(target);
                const size_t           shift = cshift;
                const unit_t xmin = r.x;
                const unit_t xmax = r.x_end;
                const unit_t ymin = r.y;
                const unit_t ymax = r.y_end;
                const size_t nch  = tgt.size();


                for(unit_t y=ymax;y>=ymin;--y)
                {
                    const typename Pixmap<TYPE>::Row &Sy = src[y];
                    for(unit_t x=xmax;x>=xmin;--x)
                    {
                        const UNIT *Syx = ((const UNIT *) &Sy[x]);
                        --Syx;
                        Syx+=shift;
                        for(size_t ch=nch;ch>0;--ch)
                        {
                            (*(tgt[ch]))[y][x] = Syx[ch];
                        }
                    }
                }
            }

            template <
            typename TYPE,
            typename UNIT>
            void mergeThread( const Area &r, threading::context &) throw()
            {
                assert(source);
                assert(target);
                Pixmap<TYPE>        &tgt   = *static_cast<Pixmap<TYPE>        *>(target);
                const Pixmaps<UNIT> &src   = *static_cast<const Pixmaps<UNIT> *>(source);
                const size_t         shift = cshift;
                const unit_t xmin = r.x;
                const unit_t xmax = r.x_end;
                const unit_t ymin = r.y;
                const unit_t ymax = r.y_end;
                const size_t nch  = src.size();


                for(unit_t y=ymax;y>=ymin;--y)
                {
                    typename Pixmap<TYPE>::Row &Sy = tgt[y];
                    for(unit_t x=xmax;x>=xmin;--x)
                    {
                        UNIT *Syx = (( UNIT *) &Sy[x]);
                        --Syx;
                        Syx+=shift;
                        for(size_t ch=nch;ch>0;--ch)
                        {
                            Syx[ch] = (*(src[ch]))[y][x];
                        }
                    }
                }
            }
        };
    }
}

#endif

