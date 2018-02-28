#ifndef YOCTO_MATH_SIG_EXPAND_INCLUDED
#define YOCTO_MATH_SIG_EXPAND_INCLUDED 1

#include "yocto/math/types.hpp"
#include "yocto/sequence/array.hpp"
#include "yocto/math/point2d.hpp"

namespace yocto
{

    namespace math
    {

        class expand
        {
        public:
            enum boundary
            {
                zero,      //!< return 0
                constant,  //!< return boundary value
                cyclic,    //!< assuming cyclic behavior
                odd,       //!< odd  behvior w.r.t the boundady
                even       //!< even behavior w.r.t the boundary
            };
            const boundary lower;
            const boundary upper;

            inline explicit expand(const boundary lo, const boundary up) throw() :
            lower(lo), upper(up) {}

            inline explicit expand(const boundary both) throw() :
            lower(both), upper(both) {}

            inline virtual ~expand() throw() {}

            inline boundary derivative_boundary( const boundary t ) throw()
            {
                switch(t)
                {
                    case cyclic: return cyclic;
                    case odd:    return even;
                    case even:   return odd;
                    default: break;
                }
                return zero;
            }

            template <typename T> inline
            T get_x(unit_t i, const array<T> &X) const
            {
                const unit_t N( X.size() ); assert(N>0);
                if(i<1)
                {
                    switch(lower)
                    {

                        case cyclic:
                            return get_x(--i+N,X) - (X[N]-X[1]);

                        default:
                            return X[1]+(X[1] - get_x(2-i, X));
                    }
                }
                else
                {
                    if(i>N)
                    {
                        switch(upper)
                        {
                            case cyclic:
                                return get_x(++i-N,X)+(X[N]-X[1]);

                            default:
                                return X[N] + (X[N] - get_x(N+N-i,X));
                        }
                    }
                    else
                    {
                        return X[i];
                    }
                }
                return X[i];
            }

            template <typename T> inline
            T get_y(unit_t i, const array<T> &Y) const
            {
                const unit_t N( Y.size() ); assert(N>0);
                if(i<1)
                {
                    switch(lower)
                    {
                        case zero:
                            return 0;

                        case constant:
                            return Y[1];

                        case cyclic:
                            return get_y(--i+N,Y);

                        case odd:{
                            const T Y1 = Y[1];
                            return  Y1 - (get_y(2-i,Y)-Y1);
                        }

                        case even:
                            return get_y(2-i,Y);
                    }
                }
                else
                {
                    if(i>N)
                    {
                        switch(upper)
                        {
                            case zero:
                                return 0;

                            case constant:
                                return Y[N];

                            case cyclic:
                                return get_y(++i-N,Y);

                            case odd: {
                                const T YN = Y[N];
                                return  YN - (get_y(N+N-i,Y)-YN);
                            }

                            case even:
                                return get_y(N+N-i,Y);

                        }
                    }
                    else
                    {
                        return Y[i];
                    }
                }
            }

            template <typename T>
            point2d<T> get( unit_t i, const array<T> &X, const array<T> &Y ) const
            {
                assert(X.size()==Y.size());
                return point2d<T>( get_x(i,X), get_y(i,Y) );
            }


        };
        

    }
}

#endif
