
#include "yocto/ink/ops/edges.hpp"
#include "yocto/ink/ops/histogram.hpp"

namespace yocto
{
    namespace Ink
    {

        Edges:: ~Edges() throw()
        {
        }
        

        Edges:: Edges(const unit_t W, const unit_t H) :
        Pixmap<uint8_t>(W,H),
        S(W,H),
        G(W,H),
        A(W,H),
        M(W,H),
        B(*this),
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
            engine.submit(this, &Edges::gradThread);
            const Domain *dom = engine.head();
            Gmax = dom->get<float>(0);
            for(dom=dom->next;dom;dom=dom->next)
            {
                const float tmp = dom->get<float>(0);
                if(tmp>Gmax) Gmax=tmp;
            }
        }


        void Edges:: gradThread( const Domain &dom, threading::context & ) throw()
        {

            YOCTO_INK_AREA_LIMITS(dom);
            float vmax=0;

            for(unit_t j=ymax;j>=ymin;--j)
            {
                const unsigned            jpos = this->ypos(j);
                const Pixmap<float>::Row &Sj   = S[j];
                for(unit_t i=xmax;i>=xmin;--i)
                {
                    float          Gx  = 0;
                    float          Gy  = 0;
                    const unsigned ipos = S.xpos(i);
                    if(0==ipos)
                    {
                         Gx = Sj[i+1]   - Sj[i-1];
                    }
                    if(0==jpos)
                    {
                         Gy = S[j+1][i]-S[j-1][i];
                    }
#if 0
                    const float    f0  = Sj[i];
                    // X component
                    switch(S.xpos(i))
                    {
                        case AtLowerX: {
                            const type f1 = Sj[i+1];
                            const type f2 = Sj[i+2];
                            Gx = 4*f1-(3*f0+f2);
                        } break;
                        case AtUpperX: {
                            const type f1 = Sj[i-1];
                            const type f2 = Sj[i-2];
                            Gx = (3*f0+f2)-4*f1;
                        } break;
                        default:
                            assert(0==xpos(i));
                            Gx = Sj[i+1]   - Sj[i-1];
                            break;
                    }

                    // Y component
                    switch(jpos)
                    {
                        case AtLowerY: {
                            const type f1 = S[j+1][i];
                            const type f2 = S[j+2][i];
                            Gy = 4*f1-(3*f0+f2);
                        } break;
                        case AtUpperY : {
                            const type f1 = S[j-1][i];
                            const type f2 = S[j-2][i];
                            Gy = (3*f0+f2)-4*f1;
                        } break;
                        default:
                            assert(0==jpos);
                            Gy = S[j+1][i]-S[j-1][i];
                            break;
                    }
#endif
                    
                    // storing component
                    const float gnorm = math::Hypotenuse(Gx,Gy);
                    const float angle = math::Atan2(Gy,Gx);
                    G[j][i] = gnorm;
                    A[j][i] = angle;
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
            engine.prepare( Histogram::BytesPerDomain );
            if(Gmax>0)
            {
                engine.submit_no_flush(this, &Edges::keepThread );
                Histogram H;
                engine.flush();
                for(const Domain *dom = engine.head(); dom; dom=dom->next)
                {
                    Histogram::count_t *bins = static_cast<Histogram::count_t *>(dom->cache.data);
                    for(size_t i=0;i<Histogram::BINS;++i)
                    {
                        H.hist[i] += bins[i];
                    }
                }
                
                strong = H.threshold();
                weak   = (strong>>1);
                
            }
            else
            {
                M.ldz();
                this->ldz();
                strong=weak=0;
            }
        }

        static inline float __probe(const Pixmap<float> &I,
                                    const unit_t         x,
                                    const unit_t         y,
                                    const float          angle) throw()
        {
            assert(I.has(x,y));
            const unit_t dx = unit_t(floorf(cosf(angle)+0.5f));
            const unit_t dy = unit_t(floorf(sinf(angle)+0.5f));
            assert(1==max_of( abs_of(dx), abs_of(dy) ));
            const coord p(x+dx,y+dy);
            if(I.has(p))
            {
                return I[p];
            }
            else
            {
                return I[y][x];
            }
        }

        void Edges:: keepThread( const Domain &dom, threading::context & ) throw()
        {
            static const float __pi = math::numeric<float>::pi;
            YOCTO_INK_AREA_LIMITS(dom);
            const float         scal = 255.0f / Gmax;
            Histogram::count_t *bins = static_cast<Histogram::count_t *>(dom.cache.data);
            for(unit_t j=ymax;j>=ymin;--j)
            {
                const Pixmap<float>::Row &Gj   = G[j];
                const Pixmap<float>::Row &Aj   = A[j];
                Pixmap<uint8_t>::Row     &Mj   = M[j];
                for(unit_t i=xmax;i>=xmin;--i)
                {
                    const float g0      = Gj[i];
                    const float g_front = __probe(G,i,j,Aj[i]);
                    const float g_back  = __probe(G,i,j,Aj[i]+__pi);
                    if(g_front<=g0&&g_back<=g0)
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
        
        void Edges:: connect(Particles &particles, Engine &engine)
        {
            // find who's who's according to weak and strong
            engine.submit(this, & Edges::connThread );
            B.build(*this,particles,true);
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
            B.rewrite(particles);
            
        }
        
        void Edges:: connThread(const Domain &dom, threading::context &) throw()
        {
            YOCTO_INK_AREA_LIMITS(dom);
            for(unit_t j=ymax;j>=ymin;--j)
            {
                const Pixmap<uint8_t>::Row     &Mj   = M[j];
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
