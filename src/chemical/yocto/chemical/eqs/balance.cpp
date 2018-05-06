#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/symdiag.hpp"

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
            std::cerr << "NuTNu=" << NuTNu << std::endl;
            
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
                return true;
            }
            else
            {
                // compute the descent direction
                std::cerr << "C   =" << C    << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                tao::mul(dC,NuTNu,beta);
                std::cerr << "dC  =" << dC   << std::endl;

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
                    }
                    else if(d<0)
                    {
                        //______________________________________________________
                        //
                        // check decreasing concentration
                        //______________________________________________________
                    }

                }


                return false;
            }



        }



    }

}

