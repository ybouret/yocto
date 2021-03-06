#ifndef YOCTO_MATH_OPT_CGRAD_INCLUDED
#define YOCTO_MATH_OPT_CGRAD_INCLUDED 1

#include "yocto/math/fcn/gradient.hpp"

namespace yocto
{
    namespace math
    {

        template <typename T>
        class cgrad : public _gradient<T>
        {
        public:
            typedef functor<bool,TL1(const array<T>)> callback;


            //! return false only if callback returns false !
            /**
             this is the standalone version.
             \param func an objective function
             \param grad its gradient
             \param p starting point
             \param ftol fractional tolerance on f and p
             \param cb   an optional callback
             */
            static bool optimize(typename numeric<T>::scalar_field &func,
                                 typename numeric<T>::vector_field &grad,
                                 array<T>                          &p,
                                 const array<bool>                 &used,
                                 const T                            ftol,
                                 callback                          *cb = 0
                                 );


            explicit cgrad();
            virtual ~cgrad() throw();

            //! return false only if callback returns false !
            /**
             The gradient is internally computed.
             \param func an objective function
             \param p   starting point
             \param dp  scaling for derivatives evaluation
             \param cb  an optional callback
             */
            bool run(typename numeric<T>::scalar_field &func,
                     array<T>                          &p,
                     const array<bool>                 &used,
                     const array<T>                    &dp,
                     const T                            ftol,
                     callback                          *cb = 0
                     );

            void compute_error(array<T>                          &perr,
                               typename numeric<T>::scalar_field &func,
                               const array<T>                    &p,
                               const array<T>                    &dp);


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(cgrad);
            typename numeric<T>::vector_field  G;
            const array<T>                    *pdp;

            void compute_gradient( array<T> &dFdx, const array<T> &x );

        };



    }
}

#endif
