
#include "yocto/ink/ops/edges.hpp"
#include "yocto/ink/ops/histogram.hpp"
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
        gradient(W,H),
        direction(W,H),
        maxima(W,H),
        tags(*this),
        Gmax(0),
        strong(0),
        weak(0)
        {

        }

        ////////////////////////////////////////////////////////////////////////
        //
        // Gradient
        //
        ////////////////////////////////////////////////////////////////////////
        void Edges:: computeGradient(Engine &engine)
        {
            engine.prepare( max_of<size_t>(sizeof(Gmax), Histogram::BytesPerDomain ) );
            engine.submit(this, &Edges::gradientThread);
            const Domain *dom = engine.head();
            Gmax = dom->get<float>(0);
            for(dom=dom->next;dom;dom=dom->next)
            {
                const float tmp = dom->get<float>(0);
                if(tmp>Gmax) Gmax=tmp;
            }
        }


        void Edges:: gradientThread( const Domain &dom, threading::context & ) throw()
        {

            YOCTO_INK_AREA_LIMITS(dom);
            float vmax=0;


            for(unit_t j=ymax;j>=ymin;--j)
            {
                const bool                compute_y = (0 == ypos(j));
                const Pixmap<float>::Row &Ij        = intensity[j];
                Pixmap<float>::Row       &Gj        = gradient[j];
                Pixmap<float>::Row       &Aj        = direction[j];
                for(unit_t i=xmax;i>=xmin;--i)
                {
                    float          Gx  = 0;
                    float          Gy  = 0;
                    const unsigned ipos = intensity.xpos(i);
                    if(0==ipos)
                    {
                         Gx = Ij[i+1]   - Ij[i-1];
                    }
                    if(compute_y)
                    {
                         Gy = intensity[j+1][i]-intensity[j-1][i];
                    }

                    // storing component
                    const float gnorm = math::Hypotenuse(Gx,Gy);
                    const float angle = math::Atan2(Gy,Gx);
                    Gj[i] = gnorm;
                    Aj[i] = angle;
                    if(gnorm>vmax) vmax=gnorm;
                }
            }
            dom.get<float>(0) = vmax;
        }
        
        ////////////////////////////////////////////////////////////////////////
        //
        // local Maxima
        //
        ////////////////////////////////////////////////////////////////////////
        void Edges:: keepLocalMaxima(Engine &engine)
        {

            if(Gmax>0)
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
            assert(I.has(x,y));
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
                if(I.has(p)) fwd = I[p];
            }

            // check rev
            {
                const unit_t dx = unit_t(floorf(0.5f-c));
                const unit_t dy = unit_t(floorf(0.5f-s));
                assert(1==max_of( abs_of(dx), abs_of(dy) ));
                const coord p(x+dx,y+dy);
                if(I.has(p)) rev = I[p];
            }

            return (fwd<=mid)&&(rev<=mid);
        }



        void Edges:: maximaThread( const Domain &dom, threading::context & ) throw()
        {
            //static const float __pi = math::numeric<float>::pi;
            YOCTO_INK_AREA_LIMITS(dom);
            const float         scal = 255.0f / Gmax;
            Histogram::count_t *bins = static_cast<Histogram::count_t *>(dom.cache.data);
            for(unit_t j=ymax;j>=ymin;--j)
            {
                const Pixmap<float>::Row &Gj   = gradient[j];
                const Pixmap<float>::Row &Aj   = direction[j];
                Pixmap<uint8_t>::Row     &Mj   = maxima[j];
                for(unit_t i=xmax;i>=xmin;--i)
                {
                    const float g0      = Gj[i];
                    if(__is_local_max(g0,gradient, i, j, Aj[i]))
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

        ////////////////////////////////////////////////////////////////////////
        //
        // grading from maxima, weak and strong values
        //
        ////////////////////////////////////////////////////////////////////////
        void Edges:: grade( Engine &engine )
        {
            engine.submit(this, & Edges::gradeThread );
        }


        ////////////////////////////////////////////////////////////////////////
        //
        // particle connections
        //
        ////////////////////////////////////////////////////////////////////////
        void Edges:: connect(Particles &particles, Engine &engine)
        {
            tags.build(*this,particles,true);
            std::cerr << "detected "  << particles.size << " edges" << std::endl;
            
            //return;
           
            Pixmap<uint8_t> &E = *this;
            Particles        tmp;
            while( particles.size )
            {
                Particle *p = particles.pop_front();
                bool is_strong = false;
                for(const Vertex *v = p->head;v;v=v->next)
                {
                    if( STRONG == E[v->pos] )
                    {
                        is_strong = true;
                        break;
                    }
                }
                
                if(is_strong)
                {
                    for(const Vertex *v = p->head;v;v=v->next)
                    {
                        E[v->pos] = STRONG;
                    }
                    tmp.push_back(p);
                }
                else
                {
                    for(const Vertex *v = p->head;v;v=v->next)
                    {
                        E[v->pos] = WEAK;
                    }
                    tmp.push_back(p);
                    //delete p;
                }
            }
            particles.swap_with(tmp);
            std::cerr << "remaining "  << particles.size << " edges" << std::endl;
            tags.rewrite(particles);
            
        }
        
        void Edges:: gradeThread(const Domain &dom, threading::context &) throw()
        {
            YOCTO_INK_AREA_LIMITS(dom);
            for(unit_t j=ymax;j>=ymin;--j)
            {
                const Pixmap<uint8_t>::Row     &Mj   = maxima[j];
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
        
    }
}
