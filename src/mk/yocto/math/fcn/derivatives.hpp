#ifndef YOCTO_MATH_DERIVATIVES_INCLUDED
#define YOCTO_MATH_DERIVATIVES_INCLUDED 1

#include "yocto/math/types.hpp"

namespace yocto
{
    namespace math
    {

        template <typename T>
        class derivatives
        {
        public:
            typedef typename numeric<T>::function scalar_function;

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

            T diff( scalar_function &f, const T x, const T h, T &err);

            //! best effort
            T compute( scalar_function &f, const T x, const T h);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(derivatives);
            void *impl;
            
        public:
            const T max_ftol;   //!< maximum possible fractional accuracy: sqrt(epsilon)
            const T opt_step;   //!< optimal step: h = opt_step * (characteristic function scale)
        };

    }
}

#endif

