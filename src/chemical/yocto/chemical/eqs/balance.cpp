#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/symdiag.hpp"
#include "yocto/sort/quick.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {

        void equilibrium::compute_extents( range &fwd, range &rev, const array<double> &C) const throw()
        {
            compute_forward(fwd,C);
            compute_reverse(rev,C);
        }

        void equilibrium:: compute_forward( range &fwd, const array<double> &C) const throw()
        {
            fwd.clear();

            for(const actor *a=reactants.head;a;a=a->next)
            {
                const size_t id = a->sp->indx;
                const double c  = C[id];
                if(c<0)
                {
                    // blocked
                    fwd.exists=true;
                    fwd.xi    =0;
                    fwd.id    =0;
                    fwd.no    =id;
                }
                else
                {
                    const int    nu = a->nu;   assert(nu<0);
                    const double xi = c/(-nu); assert(xi>=0);
                    if( (!fwd.exists) || (xi<fwd.xi) )
                    {
                        fwd.exists = true;
                        fwd.xi     = xi;
                        fwd.id     = id;
                    }
                }
            }
        }

        void equilibrium:: compute_reverse( range &rev, const array<double> &C) const throw()
        {

            for(const actor *a=products.head;a;a=a->next)
            {
                const size_t id = a->sp->indx;
                const double c  = C[id];
                if(c<0)
                {
                    // blocked
                    rev.exists=true;
                    rev.xi    =0;
                    rev.id    =0;
                    rev.no    =id;
                }
                else
                {
                    const int    nu = a->nu; assert(nu>0);
                    const double xi = -c/(nu); assert(xi<=0);
                    if( (!rev.exists) || (xi>rev.xi) )
                    {
                        rev.exists = true;
                        rev.xi     = xi;
                        rev.id     = id;
                    }
                }
            }
        }




        bool equilibria:: balance(array<double> &C0) throw()
        {
            vector<double> score(N,0);
            vector<size_t> imove(N,0);
        BALANCE:
            //__________________________________________________________________
            //
            // build the vector of bad concentrations
            //__________________________________________________________________
            size_t nbad      = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = C0[j];
                if(active[j]&&(Cj<0))
                {
                    ++nbad;
                    beta[j] = -Cj;
                }
            }
            if(nbad<=0)
            {
                //______________________________________________________________
                //
                // balanced return!
                //______________________________________________________________
                return true;
            }
            else
            {
                //______________________________________________________________
                //
                // compute the descent direction
                //______________________________________________________________
                std::cerr << "C   =" << C    << std::endl;
                std::cerr << "beta=" << beta << std::endl;
#if 0
                for(size_t i=N;i>0;--i)
                {
                    score[i] = fabs(tao::dot(beta,Nu[i])/sqrt(nu2[i]));
                    imove[i] = i;
                }
                std::cerr << "score=" << score << std::endl;
                co_qsort(score,imove,__compare_decreasing<double>);
                std::cerr << "imove=" << imove << std::endl;
                exit(0);
#endif
                // on the Nu space
                tao::mul(xi,Nu,beta);
                for(size_t i=N;i>0;--i)
                {
                    xi[i] /= nu2[i]; // rescaling according to topology
                }
                // on the Nu space
                tao::mul(dC,NuT,xi);
                std::cerr << "desc=" << dC   << std::endl;

                //______________________________________________________________
                //
                // compute the descent factor
                //______________________________________________________________
                size_t increase_idx = 0;
                double increase_fac = 0;
                size_t decrease_idx = 0;
                double decrease_fac = 0;
                for(size_t j=M;j>0;--j)
                {
                    if(!active[j]) continue;
                    const double d = dC[j];
                    const double c = C[j];

                    if(d>0)
                    {
                        //______________________________________________________
                        //
                        // check increasing concentration
                        //______________________________________________________
                        if(c<0)
                        {
                            //__________________________________________________
                            //
                            // don't go too fast
                            //__________________________________________________
                            const double fac = (-c)/d;
                            std::cerr << "INCR: C[" << j << "]=" << c << " : d=" << d << " : " << fac << std::endl;
                            if( (!increase_idx) || (fac>increase_fac) )
                            {
                                increase_idx = j;
                                increase_fac = fac;
                            }
                        }
                        // else do nothing!
                    }
                    else if(d<0)
                    {
                        //______________________________________________________
                        //
                        // check decreasing concentration
                        //______________________________________________________
                        if(c<=0)
                        {
                            //__________________________________________________
                            //
                            // blocked!
                            //__________________________________________________
                            std::cerr << "BLCK: C[" << j << "]=" << c << std::endl;
                            return false;
                        }
                        else
                        {
                            //__________________________________________________
                            //
                            // don't make a negative concentration!
                            //__________________________________________________
                            assert(c>0);
                            const double fac = c/(-d);
                            std::cerr << "DECR: C[" << j << "]=" << c << " : d=" << d << " : " << fac << std::endl;
                            if( !(decrease_idx) || (fac<decrease_idx) )
                            {
                                decrease_idx = j;
                                decrease_fac = fac;
                            }
                        }
                    }
                }

                //______________________________________________________________
                //
                // choose the step factor
                //______________________________________________________________
                if(!increase_idx)
                {
                    std::cerr << "coulnd't increase...failure!" << std::endl;
                    return false;
                }
                std::cerr << "increase@" << increase_idx << " : " << increase_fac << std::endl;
                double alpha = increase_fac;
                size_t zindx = increase_idx;
                if( decrease_idx )
                {
                    std::cerr << "decrease@" << decrease_idx << " : " << decrease_fac << std::endl;
                    if(decrease_fac<increase_fac)
                    {
                        std::cerr << "->takes over!" << std::endl;
                        alpha = decrease_fac;
                        zindx = decrease_idx;
                    }
                }
                std::cerr << "alpha=" << alpha << ", zindx=" << zindx << std::endl;

                //______________________________________________________________
                //
                // carefull move
                //______________________________________________________________
                for(size_t j=M;j>0;--j)
                {
                    if(!active[j]) { assert(Fabs(dC[j])<=0); continue; }
                    const double d = alpha * dC[j];
                    C[j] += d;
                    if(d<=0&&C[j]<=0)
                    {
                        C[j] = 0;
                    }
                }
                C[zindx] = 0;
                goto BALANCE;
            }
        }



    }
}

