#include "yocto/math/fcn/newton.hpp"
#include "yocto/math/ztype.hpp"
#include "yocto/exception.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/ios/ocstream.hpp"

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
            assert(pF);
            tao::setprobe(Xtry,X0,u,p);
            (*pF)(Ftry,Xtry);
            return __NEWTON_f(Ftry);
        }

        template <>
        Newton<real_t>::Newton(const size_t n) :
        nvar(__Newton_check_dim(n)),
        alpha(REAL(1.0e-4)),
        X0(nvar),
        F0(nvar),
        J0(nvar),
        p(nvar),
        G0(nvar),
        Xtry(nvar),
        Ftry(nvar),
        step(nvar),
        pF(0),
        sigma(0),
        g(this, & Newton<real_t>::call_g )
        {
            
        }

        template <>
        bool Newton<real_t>::accept_step(const real_t forg,const real_t ftry) throw()
        {
            tao::setvec(step,X0,Xtry);
            const double rate = min_of<real_t>(tao::dot(G0,step),0);
            std::cerr << "forg=" << forg << ", rate=" << rate << ", ftry=" << ftry << ", accept@" << forg+alpha*rate << std::endl;
            return ftry <= forg + alpha * rate;
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
            for(size_t i=nvar;i>0;--i)
            {
                X0[i] = X[i];
            }
            pF = &F;
            F(F0,X0);
            J(J0,X0);
            double f0 = __NEWTON_f(F0);

            //__________________________________________________________________
            //
            //
            // Main Loop
            //
            //__________________________________________________________________
        NEWTON_STEP:
            std::cerr << "X0=" << X0 << std::endl;
            std::cerr << "F0=" << F0 << std::endl;
            std::cerr << "J0=" << J0 << std::endl;
            std::cerr << "f0=" << f0 << std::endl;
            tao::mul_trn(G0,J0,F0);
            if(!LU<real_t>::build(J0))
            {
                // singular position
                return false;
            }

            tao::neg(p,F0);
            LU<real_t>::solve(J0,p);
            std::cerr << "p=" << p << std::endl;
            const real_t sigma = -tao::dot(G0,p);
            std::cerr << "sigma=" << sigma << std::endl;
            if( sigma <= REAL(0.0) )
            {
                // spurious position
                return false;
            }

            //__________________________________________________________________
            //
            // try the full step
            //__________________________________________________________________
            double f1 = g(1.0);
            std::cerr << "f1=" << f1 << "@Xtry=" << Xtry << ", Ftry=" << Ftry << std::endl;
            if( accept_step(f0,f1) )
            {
                //______________________________________________________________
                //
                // accept the step and study stop condition
                //______________________________________________________________
                std::cerr << "// ACCEPT" << std::endl;
                tao::set(X0,Xtry);
                tao::set(F0,Ftry);
                // analyze stop condition
                if(f1>=f0)
                {
                    goto NEWTON_DONE;
                }
                f0 = f1;
                J(J0,X0);
                goto NEWTON_STEP;
            }
            else
            {
                //______________________________________________________________
                //
                // need to backtrack
                //______________________________________________________________
                static const real_t shrink = numeric<real_t>::gold_inv;
                assert(shrink<REAL(1.0));
                assert(shrink>REAL(0.0));

                std::cerr << "// BACKTRACKING" << std::endl;
                //alpha = 0.99;
                assert(f1>=f0);

                const real_t beta = (f1-f0)+sigma;
                const real_t num  = sigma;
                const real_t den  = beta+beta+numeric<real_t>::ftol;
                real_t       lam  = num/den;
                f1 = g(lam);
                std::cerr << "f(" << lam << ")=" << f1 << "/" << f0 << std::endl;

                real_t lam0 = lam;
                while( !accept_step(f0,f1) )
                {
                    lam *= shrink;
                    if(lam>=lam0)
                    {
                        std::cerr << "couldn't backtrack..." << std::endl;
                        return false;
                    }
                    std::cerr << "f(" << lam << ")=" << f1 <<  std::endl;
                    lam0 = lam;
                }
                std::cerr << "accepted..." << std::endl;
                exit(1);

            }

        NEWTON_DONE:
            for(size_t i=nvar;i>0;--i)
            {
                X[i] = X0[i];
            }

            return true;
        }


    }
}
