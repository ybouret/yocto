#ifndef YOCTO_MATH_DERIVATIVES_INCLUDED
#define YOCTO_MATH_DERIVATIVES_INCLUDED 1

#include "yocto/math/types.hpp"
#include "yocto/container/matrix.hpp"

namespace yocto
{
    namespace math
    {

        template <typename T>
        class derivatives : public object
        {
        public:
            typedef typename numeric<T>::function                function;
            typedef typename numeric<T>::scalar_field            scalar_field;
            typedef typename numeric<T>::parametric_function     parametric_function;

            explicit derivatives();
            virtual ~derivatives() throw();

            static bool regularize(const T x,T&h) throw();


            //! Returns the derivative of a function  by Ridders’ method.
            /**
             \param f   a derivable function
             \param x   the point where to evaluate
             \param h   input as an estimated initial stepsize; it need not be small, but rather should be an increment in x over which func changes substantially
             \param err estimate of the error
             */

            T diff(  function &f, const T x, const T h, T &err);

            //! best effort
            T compute( function &f, const T x, const T h);

            //! derivative of f w.r.t x for a given set of parameters
            T compute( parametric_function &f, const T x, const array<T> &param, const T h );

            //! gradient. NOT thread safe, uses x as local memory...
            void gradient( array<T> &grad, scalar_field &f, const array<T> &x, const array<T> &h );

            //! gradient. NOT thread safe, uses x as local memory...
            void gradient( array<T> &grad, parametric_function &f, const T u, const array<T> &x, const array<T> &h );



        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(derivatives);
            matrix<T>            a;
            size_t               ivar;
            void                *call;
            const array<T>      *vptr;
            function             fdir; //!< field     direction, for gradient
            function             fpar; //!< parameter direction, for derivative w.r.t parameter
            function             ffit; //!< uses parametric_field as fit function
            T                    vpar; //!< value of  parameter
            T                    eval_field( T xx); //!< (*fptr)( (*vptr)@ivar=x )
            T                    eval_pfunc( T xx); //!< (*pptr)(xx, (*vptr) )
            T                    eval_fitfn( T xx); //!< (*pptr)(vpar,(*vptr)@ivar=x )
            
        public:
            const T max_ftol;   //!< maximum possible fractional accuracy: sqrt(epsilon)
            const T opt_step;   //!< optimal step: h = opt_step * (characteristic function scale)
        };

    }
}

#endif

