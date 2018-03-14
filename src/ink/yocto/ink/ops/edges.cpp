
#include "yocto/ink/ops/edges.hpp"
#include "yocto/ink/ops/histogram.hpp"
#include "yocto/ink/ops/mapper.hpp"
#include "yocto/ios/ocstream.hpp"

namespace yocto
{
    namespace Ink
    {
        Edges:: ~Edges() throw()
        {
        }

        Edges:: Edges(const unit_t W, const unit_t H) :
        Pixmap<uint8_t>(W,H),
        intensity(W,H),
        gradient(*this),
        maxima(W,H),
        tags(*this),
        temporary(W,H),
        strong(0),
        weak(0)
        {
        }

        void Edges:: loadIntensity(const Pixmap<float> &source, Blur *blur, Engine &engine)
        {
            if(blur)
            {
                blur->apply(intensity, temporary, source, engine);
            }
            else
            {
                Mapper mapper;
                mapper(intensity,source,Convert::Copy<float,float>,engine);
            }
        }

        void Edges:: computeGradient(const Stencil &dx,
                                     const Stencil &dy,
                                     Engine &engine)
        {
            gradient.compute(intensity,dx,dy,engine,false);
        }

        ////////////////////////////////////////////////////////////////////////
        //
        // local Maxima
        //
        ////////////////////////////////////////////////////////////////////////
        void Edges:: keepLocalMaxima(Engine &engine)
        {
            if(gradient.v_max>0)
            {
                engine.prepare( Histogram::BytesPerDomain );
                engine.submit_no_flush(this, &Edges::maximaThread );
                Histogram H;
                engine.flush();
                H.collectFrom(engine);
                strong = H.threshold();
                weak   = (strong>>1);
                {
                    ios::wcstream fp("edges_hist.dat");
                    for(size_t i=0;i<Histogram::BINS;++i)
                    {
                        fp("%g %g\n", double(H.Bins[i]), double(H.counts[i]));
                    }

                }
            }
            else
            {
                maxima.ldz();
                this->ldz();
                strong=weak=0;
            }
        }

        static inline bool __is_local_max(const float          I0,
                                          const Pixmap<float> &I,
                                          const unit_t         x,
                                          const unit_t         y,
                                          const float          angle) throw()
        {
            assert(I.contains(x,y));
            assert( *(uint32_t *)&I0 == *(uint32_t *)&I[y][x]);
            const float c   = cosf(angle);
            const float s   = sinf(angle);
            const float mid = I0;
            float       fwd = mid;
            float       rev = mid;
            // check fwd
            {
                const unit_t dx = unit_t(floorf(c+0.5f));
                const unit_t dy = unit_t(floorf(s+0.5f));
                assert(1==max_of( abs_of(dx), abs_of(dy) ));
                const coord p(x+dx,y+dy);
                if(I.contains(p))
                {
                    fwd = I[p];
                }

            }


            // check rev
            {
                const unit_t dx = unit_t(floorf(0.5f-c));
                const unit_t dy = unit_t(floorf(0.5f-s));
                assert(1==max_of( abs_of(dx), abs_of(dy) ));
                const coord p(x+dx,y+dy);
                if(I.contains(p))
                {
                    rev = I[p];
                }
            }

            return (fwd<=mid)&&(rev<=mid);
        }



        void Edges:: maximaThread( const Domain &dom, threading::context & ) throw()
        {
            YOCTO_INK_AREA_LIMITS(dom);
            const float         scal = 255.0f / gradient.v_max;
            Histogram::count_t *bins = static_cast<Histogram::count_t *>(dom.cache.data);
            for(unit_t j=ymax;j>=ymin;--j)
            {
                const Pixmap<float>::Row &Gj   = gradient.value[j];
                const Pixmap<float>::Row &Aj   = gradient.angle[j];
                Pixmap<uint8_t>::Row     &Mj   = maxima[j];
                for(unit_t i=xmax;i>=xmin;--i)
                {
                    const float g0      = Gj[i];
                    if(__is_local_max(g0,gradient.value, i, j, Aj[i]))
                    {
                        const uint8_t b = uint8_t(floorf(scal*g0+0.5f));
                        Mj[i] = b;
                        ++bins[b];
                    }
                    else
                    {
                        Mj[i] = 0;
                    }
                }

            }
        }


        void  Edges:: statisticalLevels(Engine &engine)
        {
            engine.submit(this, &Edges::statisticalThread);
        }

        void Edges:: statisticalThread(const Domain &dom, threading::context &) throw()
        {
            YOCTO_INK_AREA_LIMITS(dom);
            for(unit_t j=ymax;j>=ymin;--j)
            {
                const Pixmap<uint8_t>::Row     &Mj   =  maxima[j];
                Pixmap<uint8_t>::Row           &Ej   = (*this)[j];
                for(unit_t i=xmax;i>=xmin;--i)
                {
                    const uint8_t m = Mj[i];
                    if(m<=weak)
                    {
                        Ej[i] = 0;
                        continue;
                    }

                    if(m<strong)
                    {
                        Ej[i] = WEAK;
                        continue;
                    }

                    Ej[i] = STRONG;
                }

            }
        }

        void  Edges:: build(Particles &particles, Particles *weaks)
        {
            Pixmap<uint8_t> &self = *this;
            Particles        all;
            tags.build(*this,all,true);
            if(weaks)
            {
                weaks->clear();
            }
            
            while(all.size>0)
            {
                Particle *pt        = all.pop_front();
                bool      is_strong = false;
                for(const Vertex *vtx=pt->head;vtx;vtx=vtx->next)
                {
                    if(STRONG==self[vtx->pos])
                    {
                        is_strong = true;
                        break;
                    }
                }

                if(is_strong)
                {
                    for(const Vertex *vtx=pt->head;vtx;vtx=vtx->next)
                    {
                        self[vtx->pos] = STRONG;
                    }
                    particles.push_back(pt);
                }
                else
                {
                    if(weaks)
                    {
                        weaks->push_back(pt);
                    }
                    else
                    {
                        for(const Vertex *vtx=pt->head;vtx;vtx=vtx->next)
                        {
                            self[vtx->pos] = 0;
                        }
                        delete pt;
                    }
                }
            }
            tags.rewrite(particles);
            if(weaks)
            {
                tags.rewrite(*weaks,particles.size);
            }
        }

        void Edges::detect(Particles           &particles,
                           Particles           *weaks,
                           const Pixmap<float> &source,
                           const Stencil       &dx,
                           const Stencil       &dy,
                           Blur                *blur,
                           Engine              &engine)
        {

            loadIntensity(source,blur,engine);
            computeGradient(dx,dy,engine);
            keepLocalMaxima(engine);
            statisticalLevels(engine);
            build(particles,weaks);
        }
        
    }
}
