
#ifndef YOCTO_INK_OPS_HIST_INCLUDED
#define YOCTO_INK_OPS_HIST_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ink/pixmap.hpp"
#include "yocto/ink/color/pixel.hpp"
#include "yocto/code/bzset.hpp"

namespace yocto
{
    namespace Ink
    {
        class Histogram
        {
        public:
            enum KeepMode
            {
                KeepLT,  //!< keep background, strictly
                KeepLEQ, //!< keep background, largely
                KeepGT,  //!< keep foreground, strictly
                KeepGEQ  //!< keep foreground, largely
            };

            typedef size_t count_t;

            static const size_t  BINS           = 256;
            static const size_t  BytesPerDomain = BINS * sizeof(count_t);
            static const uint8_t Bins[BINS];
            count_t              counts[BINS];

            explicit Histogram() throw();
            virtual ~Histogram() throw();
            void     clear() throw();


            //! collect from another BINS hist
            void collect( const count_t *other ) throw();

            //! collect from domains
            void collectFrom( const Engine &engine) throw();

            //__________________________________________________________________
            //
            //! append data count, need to be cleared for one pxm
            //__________________________________________________________________
            template <typename T,typename FUNC>
            inline Histogram &build(const Pixmap<T> &pxm,
                                    FUNC            &pixel2byte,
                                    Engine          &engine)
            {
                source = &pxm;
                proc   = (void *)&pixel2byte;
                //______________________________________________________________
                //
                // allocate/clear memory for histograms
                //______________________________________________________________
                engine.prepare(BytesPerDomain);
                // start building local histograms
                engine.submit_no_flush(this, & Histogram::buildThread<T,FUNC> );
                //______________________________________________________________
                //
                // clean during this time
                //______________________________________________________________
                clear();
                //______________________________________________________________
                //
                // wait for histograms to be built
                //______________________________________________________________
                engine.flush();
                //______________________________________________________________
                //
                // reduction
                //______________________________________________________________
                collectFrom(engine);
                return *this;
            }

            //______________________________________________________________
            //
            // compute Ostu's threshold
            //______________________________________________________________
            size_t threshold() const throw();

            template <typename T,typename FUNC>
            inline void keep(const size_t       level,
                             const KeepMode     kmode,
                             Pixmap<T>          &tgt,
                             const Pixmap<T>    &src,
                             FUNC               &pixel2byte,
                             Engine             &engine,
                             const bool          inverse_pixel = false)
            {
                source = &src;
                target = &tgt;
                proc   = (void *)&pixel2byte;
                _level = level;
                _kmode = kmode;
                _invpx = inverse_pixel;
                engine.submit(this, &Histogram::keepThread<T,FUNC>);
            }

            template <typename T, typename FUNC>
            inline void foreground(Pixmap<T>       &tgt,
                                   const Pixmap<T> &src,
                                   FUNC            &pixel2byte,
                                   Engine          &engine)
            {
                keep<T,FUNC>(build(src,pixel2byte,engine).threshold(),
                             KeepGEQ,
                             tgt,
                             src,
                             pixel2byte,
                             engine,
                             false);
            }

            template <typename T, typename FUNC>
            inline void background(Pixmap<T>       &tgt,
                                   const Pixmap<T> &src,
                                   FUNC            &pixel2byte,
                                   Engine          &engine,
                                   bool             inverse_pixel=true)
            {
                keep<T,FUNC>(build(src,pixel2byte,engine).threshold(),
                             KeepLEQ,
                             tgt,
                             src,
                             pixel2byte,
                             engine,
                             inverse_pixel);
            }



        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Histogram);
            const void *source;
            void       *target;
            void       *proc;
            size_t      _level;
            KeepMode    _kmode;
            bool        _invpx;

            template <typename T,typename FUNC>
            inline void buildThread( const Domain &dom, threading::context &) throw()
            {
                assert(source);
                assert(proc);
                assert(dom.cache.data);
                assert(dom.cache.size>=BytesPerDomain);

                const Pixmap<T> &pxm        = *(const Pixmap<T> *)source;
                FUNC            &pixel2byte = *(FUNC*)proc;

                const unit_t xmin = dom.x;
                const unit_t xmax = dom.x_end;

                const unit_t ymin = dom.y;
                const unit_t ymax = dom.y_end;

                count_t *h = static_cast<count_t*>(dom.cache.data);
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    const typename Pixmap<T>::Row &S_j = pxm[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        const uint8_t p = uint8_t( pixel2byte(S_j[i]) );
                        ++h[p];
                    }
                }

            }

            template <typename T,typename FUNC>
            inline void keepThread( const Domain &dom, threading::context &) throw()
            {
                assert(source);
                assert(target);
                assert(proc);
                const size_t     level    = _level;
                const KeepMode   kmode    = _kmode;
                const Pixmap<T> &src      = *(const Pixmap<T> *)source;
                Pixmap<T>       &tgt      = *(Pixmap<T>       *)target;
                FUNC            &pix2byte = *(FUNC*)proc;
                const bool       invpx    = _invpx;

                YOCTO_INK_AREA_LIMITS(dom);

                for(unit_t j=ymax;j>=ymin;--j)
                {
                    const typename Pixmap<T>::Row &S_j = src[j];
                    typename Pixmap<T>::Row       &T_j = tgt[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {

                        const T      &v   = S_j[i];
                        const size_t  p   = uint8_t( pix2byte(v) );
                        bool          res = false;
                        switch(kmode)
                        {
                            case KeepLT:  res = (p<level);  break;
                            case KeepLEQ: res = (p<=level); break;
                            case KeepGT:  res = (p>level);  break;
                            case KeepGEQ: res = (p>=level); break;
                        }
                        if(res)
                        {
                            if(invpx)
                            {
                                T_j[i] = Pixel<T>::Inverse(v);
                            }
                            else
                            {
                                T_j[i] = v;
                            }
                        }
                        else
                        {
                            bzset(T_j[i]);
                        }
                    }
                }

            }

        };
    }
}

#endif

