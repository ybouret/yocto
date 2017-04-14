#include "yocto/math/fcn/newton.hpp"
#include "yocto/math/ztype.hpp"
#include "yocto/exception.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/svd.hpp"
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
        w(nvar),
        V(nvar),
        xstep(nvar),
        gradf(nvar),
        delta(nvar),
        Xtry(nvar),
        Ftry(nvar),
        hook(0),
        g(this, & Newton<real_t>::call_g )
        {
            
        }

#if 0
        template <>
        bool Newton<real_t>::accept_step(const real_t forg,const real_t ftry) throw()
        {
            tao::setvec(step,X0,Xtry);
            const double rate = min_of<real_t>(tao::dot(G0,step),0);
            std::cerr << "forg=" << forg << ", rate=" << rate << ", ftry=" << ftry << ", accept@" << forg+alpha*rate << std::endl;
            return ftry <= forg + alpha * rate;
        }
#endif

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
            hook = &F;
            F(F0,X0);
            J(J0,X0);
            double g0 = __NEWTON_f(F0);


            // LOOP:
            if(g0<=0)
            {
                std::cerr << fn << "[success] absolute zero reached" << std::endl;
                goto SUCCESS;
            }

            // compute gradient of F*F/2
            tao::mul_trn(gradf,J0,F0);

            // compute descent direction
            if(!svd<real_t>::build(J0,w,V))
            {
                std::cerr << fn << "[failure] singular SVD!" << std::endl;
                return false;
            }


        SUCCESS:

            return true;
        }


    }
}
