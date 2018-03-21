
#ifndef YOCTO_INK_EDM_INCLUDED
#define YOCTO_INK_EDM_INCLUDED 1

#include "yocto/ink/pixmaps.hpp"
#include "yocto/ink/parallel.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/associative/key-dumper.hpp"

namespace yocto
{
    namespace Ink
    {


        class EDM : public Pixmap<float>
        {
        public:

            class Probe
            {
            public:
                typedef set<unit_t,Probe,key_dumper<unit_t> > Set;

                const unit_t u,v,d2;

                inline  Probe(const unit_t U, const unit_t V, const unit_t D2) throw() : u(U), v(V), d2(D2) {}
                inline ~Probe() throw() {}
                inline  Probe(const Probe &other) throw() : u(other.u), v(other.v), d2(other.d2) {}
                inline const unit_t & key() const throw() { return d2; }

            private:
                YOCTO_DISABLE_ASSIGN(Probe);
            };

            class Probes : public Probe::Set
            {
            public:
                inline Probes( const unit_t w ) : Probe::Set(1000000,as_capacity)
                {
                    for(unit_t u=1;u<=w;++u)
                    {
                        const unit_t u2 = u*u;
                        for(unit_t v=1;v<=u;++v)
                        {
                            const unit_t v2 = v*v;
                            const unit_t d2 = u2+v2;
                            if( !search(d2) )
                            {
                                const Probe p(u,v,d2);
                                insert__(p);
                            }
                        }
                    }
                    sort_keys_by( __compare<unit_t> );
                }

                inline virtual ~Probes() throw() {}

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Probes);
            };


            enum ScanStatus
            {
                ScanFound,   //!< scan met background
                ScanInside,  //!< scan totally inside foregroud
                ScanOutside  //!< scan totally circle outside image
            };

            const unit_t d2_max;
            float        ed_max;
            explicit EDM( const unit_t W, const unit_t H) :
            Pixmap<float>(W,H),
            d2_max(w*w+h*h),
            ed_max(0.0f),
            src(0)
            {
            }

            virtual ~EDM() throw() {}

            template <typename T>
            void compute( const Pixmap<T> &img, Engine &engine)
            {
                static const size_t BytesPerDomain = sizeof(float);
                engine.prepare(BytesPerDomain);
                src = &img;
                engine.submit(this,& EDM::computeThread<T> );
                const Domain *dom = engine.head();
                ed_max = dom->get<float>(0);
                for(dom=dom->next;dom;dom=dom->next)
                {
                    ed_max = max_of(ed_max,dom->get<float>(0));
                }
                std::cerr << "ed_max=" << ed_max << std::endl;
            }


            
        private:
            const void  *src;
            YOCTO_DISABLE_COPY_AND_ASSIGN(EDM);
            template <typename T>
            void computeThread( const Domain &dom, threading::context &) throw()
            {
                YOCTO_INK_AREA_LIMITS(dom);
                assert(src);
                const Pixmap<T> &img  = *(const Pixmap<T> *)src;
                float            dmx  = 0.0f;
                for(unit_t ym=ymax;ym>=ymin;--ym)
                {
                    Pixmap<float>::Row &self_ym = (*this)[ym];
                    for(unit_t xm=xmax;xm>=xmin;--xm)
                    {
                        const float d = sqrtf( float(find_d2(img,xm,ym) ) );
                        self_ym[xm] = d;
                        if(d>dmx) dmx=d;
                    }
                }
                dom.get<float>(0) = dmx;
            }
            
            template <typename T>
            inline unit_t find_d2(const Pixmap<T>    &img,
                                  const unit_t  xm,
                                  const unit_t  ym)
            {
                assert(img.contains(xm,ym));
                if(Pixel<T>::IsZero(img[ym][xm]))
                {
                    return 0;
                }
                else
                {
                    for(unit_t r=1;;++r)
                    {
                        unit_t d2 = d2_max;
                        switch( scanCircle(d2, img, xm, ym, r) )
                        {
                            case ScanFound:   return d2;
                            case ScanOutside: return d2_max;
                            case ScanInside:  break;
                        }
                    }
                }
            }
            //__________________________________________________________________
            //
            //! scan circle for the first nearby zero pixel
            //__________________________________________________________________
            template <typename T>
            inline ScanStatus scanCircle(unit_t             &d2,
                                         const Pixmap<T>    &img,
                                         const unit_t        xm,
                                         const unit_t        ym,
                                         unit_t              r) throw()
            {
                assert(r>0);
                bool has_data = false;
                //______________________________________________________________
                //
                // Initialize Algorithm
                //______________________________________________________________
                unit_t x = -r, y = 0, err = 2-2*r; /* bottom left to top right */
                do {

                    //__________________________________________________________
                    //
                    // there are four possible points at the same distance
                    //__________________________________________________________
                    const coord v[4] =
                    {
                        coord(xm-x,ym+y),  /*   I. Quadrant +x +y */
                        coord(xm-y,ym-x),  /*  II. Quadrant -x +y */
                        coord(xm+x,ym-y),  /* III. Quadrant -x -y */
                        coord(xm+y,ym+x)   /*  IV. Quadrant +x -y */
                    };

                    //__________________________________________________________
                    //
                    // testing the four points (at the same distance)
                    //__________________________________________________________
                    for(size_t i=0;i<4;++i)
                    {
                        const coord p = v[i];
                        if(img.contains(p))
                        {
                            has_data = true;
                            if( Pixel<T>::IsZero(img[p]))
                            {
                                const unit_t x2 = x*x;
                                const unit_t y2 = y*y;
                                d2 = x2+y2;
                                return ScanFound;
                            }
                        }
                    }

                    //______________________________________________________________
                    //
                    // update positions
                    //______________________________________________________________
                    r = err;
                    if (r <= y)
                    {
                        err += ++y*2+1;            /* e_xy+e_y < 0 */
                    }
                    if (r > x || err > y)
                    {
                        err += ++x*2+1;
                    }
                } while (x < 0);
                return has_data ? ScanInside : ScanOutside;
            }
        };
        
    }
}

#endif

