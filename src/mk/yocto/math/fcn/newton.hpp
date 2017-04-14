#ifndef YOCTO_MK_FCN_NEWTON_INCLUDED
#define YOCTO_MK_FCN_NEWTON_INCLUDED 1

#include "yocto/math/types.hpp"
#include "yocto/container/matrix.hpp"
#include "yocto/sequence/vector.hpp"

namespace yocto
{
    namespace math
    {
        template <typename T>
        class Newton
        {
        public:
            typedef functor<void,TL2(array<T>&,const array<T>&)>            Function;
            typedef functor<void,TL2(matrix<T>&,const array<T>&)> Jacobian;

            virtual ~Newton() throw();
            explicit Newton(const size_t n);

            const size_t nvar;
            T            alpha;
            bool run( Function &F, Jacobian &J, array<T> &X );


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Newton);

            vector<T>  X0;   //!< nvar
            vector<T>  F0;   //!< nvar
            matrix<T>  J0;   //!< nvar x nvar, jacobian
            vector<T>  p;    //!< descent direction
            vector<T>  G0;   //!< grad(f) = J0'*F0
            vector<T>  Xtry;
            vector<T>  Ftry;
            vector<T>  step; //!< Xtry - X0
            Function  *pF;
            T          sigma;
            typename numeric<T>::function g;

            T call_g(T u);
            bool accept_step(const T forg,const T ftry) throw();

        };
    }
}

#endif
