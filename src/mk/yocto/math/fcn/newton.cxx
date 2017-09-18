#include "yocto/math/fcn/newton.hpp"
#include "yocto/math/ztype.hpp"
#include "yocto/exception.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/types.hxx"

namespace yocto
{
    namespace math
    {
        template <>
        Newton<real_t>:: ~Newton() throw()
        {
        }


        static const char fn[] = "Newton: ";

        static inline
        size_t __Newton_check_dim(const size_t n)
        {
            if(n<=0)
                throw exception("%sinvalid dimension",fn);
            return n;
        }

#define __NEWTON_f(vecF) ( REAL(0.5) * tao::norm_sq(vecF) )

        template <>
        real_t Newton<real_t>::call_g(const real_t u)
        {
            assert(hook);
            tao::setprobe(Xtry,X0,u,xstep);
            (*hook)(Ftry,Xtry);
            return __NEWTON_f(Ftry);
        }

        template <>
        Newton<real_t>::Newton(const size_t n) :
        nvar(__Newton_check_dim(n)),
        alpha(REAL(1.0e-4)),
        X0(nvar),
        F0(nvar),
        J0(nvar),
        xstep(nvar),
        gradf(nvar),
        delta(nvar),
        Xtry(nvar),
        Ftry(nvar),
        hook(0),
        g(this, & Newton<real_t>::call_g )
        {

        }

        template <>
        bool Newton<real_t>::accept_step(const real_t forg,const real_t ftry) throw()
        {
            tao::setvec(delta,X0,Xtry);
            const double rate = min_of<real_t>(tao::dot(gradf,delta),0);
            //std::cerr << "forg=" << forg << ", rate=" << rate << ", accept@" << forg + alpha * rate << " <= ftry=" << ftry << std::endl;
            return (ftry <= forg + alpha * rate);
        }


        template <>
        bool Newton<real_t>::xconverged() const throw()
        {

            for(size_t i=nvar;i>0;--i)
            {
                const real_t x0 = X0[i];
                const real_t x1 = Xtry[i];
                const real_t dx = Fabs(x1-x0);
                if(dx>numeric<real_t>::ftol*( max_of<real_t>(Fabs(x0),Fabs(x1)) ))
                {
                    return false;
                }
            }
            //std::cerr << "// X converged!" << std::endl;
            return true;
        }

        template <>
        bool Newton<real_t>::run( Function &F, Jacobian &J, array<real_t> &X )
        {
            assert(X.size()>=nvar);

            //__________________________________________________________________
            //
            //
            // initialize
            //
            //__________________________________________________________________
            tao::set(Xtry,X,nvar);
            hook = &F;
            F(F0,X0);
            J(J0,X0);
            double g0 = __NEWTON_f(F0);
            goto LOOP;

        SUCCESS:
            tao::set(X,Xtry,nvar);
            return true;

        LOOP:
            std::cerr << "X0=" << X0 << std::endl;
            std::cerr << "F0=" << F0 << std::endl;
            std::cerr << "J0=" << J0 << std::endl;
            std::cerr << "g0=" << g0 << std::endl;
            if(g0<=0)
            {
                std::cerr << fn << "[success] absolute zero reached" << std::endl;
                goto SUCCESS;
            }

            //__________________________________________________________________
            //
            // compute gradient of F*F/2
            //__________________________________________________________________
            tao::mul_trn(gradf,J0,F0);
            //std::cerr << "grad_f=" << gradf << std::endl;

            if(!LU<real_t>::build(J0))
            {
                std::cerr << fn << "[failure] singular coordinates" << std::endl;
                return false;
            }

            //__________________________________________________________________
            //
            // compute Newton's step
            //__________________________________________________________________
            tao::neg(xstep,F0);
            LU<real_t>::solve(J0,xstep);
            const real_t rho = -tao::dot(xstep,gradf);
            //std::cerr << "xstep=" << xstep << std::endl;
            //std::cerr << "rho  =" << rho   << std::endl;
            if(rho<=0)
            {
                std::cerr << fn << "[success] critical point" << std::endl;
                return true;
            }

            //__________________________________________________________________
            //
            // take full xstep
            //__________________________________________________________________
            double g1 = g(1);
            //std::cerr << "Xtry=" << Xtry << std::endl;
            //std::cerr << "Ftry=" << Ftry << std::endl;
            std::cerr << "g1="   << g1 << std::endl;

            //__________________________________________________________________
            //
            // check status
            //__________________________________________________________________
            if(!accept_step(g0,g1))
            {
                assert(alpha>0);
                assert(alpha<1);
                //______________________________________________________________
                //
                // first reduction
                //______________________________________________________________
                const real_t beta = (g1-g0) + rho; assert(beta>0);
                real_t       lam  = rho/( (beta+beta) + numeric<real_t>::ftol );
                //std::cerr << "lam=" << lam << std::endl;

                //______________________________________________________________
                //
                // other reductions
                //______________________________________________________________

                while( ! accept_step(g0,(g1=g(lam)) ) )
                {
                    if(xconverged())
                    {
                        // spurious case
                        std::cerr << fn << "[success] but spurious X convergence" << std::endl;
                        goto SUCCESS;
                    }
                    lam *= REAL(0.5);
                }

            }


            if(g1>=g0)
            {
                std::cerr << fn << "[success] numeric F limit reached" << std::endl;
                goto SUCCESS;
            }

            if(xconverged())
            {
                std::cerr << fn << "[success] X convergence" << std::endl;
                goto SUCCESS;
            }

            g0 = g1;
            for(size_t i=nvar;i>0;--i)
            {
                X0[i] = Xtry[i];
                F0[i] = Ftry[i];
            }
            J(J0,X0);
            goto LOOP;

        }


    }
}




