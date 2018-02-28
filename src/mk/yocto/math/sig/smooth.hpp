#ifndef YOCTO_MATH_SIG_SMOOTH
#define YOCTO_MATH_SIG_SMOOTH 1

#include "yocto/math/sig/expand.hpp"
#include "yocto/sequence/list.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/ipower.hpp"

#include "yocto/exceptions.hpp"
#include <cerrno>


namespace yocto
{
    namespace math
    {

        template <typename T>
        class smooth
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef point2d<type>     point_t;
            typedef list<point_t>     points_type;
            typedef T               (*window)(T);
            typedef typename points_type::node_type __node;
            type        lower_delta;
            type        upper_delta;
            type        total_delta;
            size_t      degree;
            points_type points;
            inline explicit smooth() throw() :
            lower_delta(0),
            upper_delta(0),
            total_delta(0),
            degree(0),
            points()
            {
            }

            inline virtual ~smooth() throw()
            {
            }

            inline void set_delta() throw()
            {
                upper_delta = max_of<T>(upper_delta,0);
                lower_delta = max_of<T>(lower_delta,0);
                total_delta = upper_delta + lower_delta;
            }

            //! assume that x is increasing
            inline void collect(const unit_t    i,
                                const expand    f,
                                const array<T> &x,
                                const array<T> &y)
            {
                assert(upper_delta>=0);
                assert(lower_delta>=0);
                assert(total_delta>=upper_delta+lower_delta);
                points.free();
                assert(x.size()==y.size());
                assert(x.size()>0);

                // get central point
                const point_t    c  = f.get(i,x,y);
                points.push_back(c);

                // get upper values
                {
                    const_type x_max= c.x + upper_delta;
                    for(unit_t j=i+1;;++j)
                    {
                        const point_t p = f.get(j,x,y);
                        assert(p.x>=c.x);
                        if(p.x>x_max) break;
                        points.push_back(p);
                    }
                }

                // get lower values
                {
                    const type x_min = c.x - lower_delta;
                    for(unit_t j=i-1;;--j)
                    {
                        const point_t p  = f.get(j,x,y);
                        assert(p.x<=c.x);
                        if(p.x<x_min) break;
                        points.push_front(p);
                    }
                }

                for( __node *node = points.head();node;node=node->next)
                {
                    node->data.x -= c.x;
                }

            }

            static inline T square( T ) throw() { return 1; }

            inline type compute( type *dYdX = 0)
            {
                assert(upper_delta>=0);
                assert(lower_delta>=0);
                assert(total_delta>=upper_delta-lower_delta);
                const size_t    np    = points.size(); assert(np>0);
                const size_t    d     = min_of(np-1,degree);
                const size_t    nvar  = d+1;
                vector<type>    coef(nvar);
                matrix<type>    mu(nvar);

                // build response and half matrix
                {
                    size_t i=0;
                    for( const __node *node = points.head();node;node=node->next)
                    {
                        ++i;
                        const point_t p  = node->data;
                        const_type    Xi = p.x;
                        const_type    Yi = p.y;
                        const_type    Wi = 1;
                        const_type    W2 = Wi*Wi;
                        for(size_t k=nvar,km=k-1;k>0;--k,--km)
                        {
                            const_type Xikm = W2 * ipower<T>(Xi,km);
                            coef[k] += Yi * Xikm;
                            for(size_t l=k,lm=l-1;l>0;--l,--lm)
                            {
                                mu[k][l] += Xikm  * ipower<T>(Xi,lm);
                            }
                        }
                    }
                }

                // symetrize
                for(size_t k=nvar;k>0;--k)
                {
                    for(size_t l=k-1;l>0;--l)
                    {
                        mu[l][k] = mu[k][l];
                    }
                }


                if( !LU<T>::build(mu) )
                {
                    throw libc::exception( EDOM, "smooth: singular data" );
                }
                LU<T>::solve(mu,coef);
                if(dYdX)
                {
                    if(nvar>=2)
                    {
                        *dYdX = coef[2];
                    }
                    else
                    {
                        *dYdX = 0;
                    }
                }
                return coef[1];
            }

            inline void operator()(const expand    &f,
                                   array<T>        &YF,
                                   const array<T>  &X,
                                   const array<T>  &Y,
                                   array<T>        *dYdX)
            {
                assert(YF.size()==X.size());
                assert(Y.size()==Y.size());
                assert(NULL==dYdX||X.size()==dYdX->size());
                const size_t N = X.size();
                set_delta();
                const bool has_dYdX = (dYdX != NULL);
                for(size_t i=N;i>0;--i)
                {
                    collect(i,f,X,Y);
                    type *d = (has_dYdX? &((*dYdX)[i]) : NULL );
                    YF[i]   = compute(d);
                }
            }
        };


    }
}


#endif
