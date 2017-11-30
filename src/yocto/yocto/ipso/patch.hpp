#ifndef YOCTO_IPSO_PATCH_INCLUDED
#define YOCTO_IPSO_PATCH_INCLUDED 1

#include "yocto/ipso/coord.hpp"
#include "yocto/code/bswap.hpp"
#include <iostream>

namespace yocto
{
    namespace ipso
    {

        template <
        typename T,
        typename COORD>
        class patch
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;

            static const size_t DIM = sizeof(COORD)/sizeof(type);

            typedef ipso::coord<T>                              __coord;
            typedef COORD                                         coord;         //!< POD coordinate
            typedef const     COORD                               const_coord;   //!< const coordinate
            typedef typename  type_traits<COORD>::parameter_type  param_coord;   //!< coordinate as parameter

            const_coord   lower; //!< lower coordinate
            const_coord   upper; //!< upper coordinate
            const_coord   width; //!< coordinate width
            const_coord   pitch; //!< coordinate pitch: (1,nx,nx*ny)
            const size_t  items; //!< number of linear items within the layout

            inline virtual ~patch() throw() {}

            inline explicit patch(const_coord lo, const coord up) throw() :
            lower(lo),
            upper(up),
            width(),
            pitch(),
            items(1)
            {
                type   *l =  (type   *)&lower;
                type   *u =  (type   *)&upper;
                type   *w =  (type   *)&width;
                size_t &n = *(size_t *)&items;
                for(size_t i=0;i<DIM;++i)
                {
                    if(l[i]>u[i]) cswap(l[i],u[i]);
                    n *= size_t( w[i] = T(1)+u[i]-l[i] );
                }
                type *p = (type *)&pitch;
                p[0] = 1;
                for(size_t i=1;i<DIM;++i)
                {
                    p[i] = p[i-1] * w[i-1];
                }
            }

            inline patch( const patch &other) throw() :
            lower(other.lower),
            upper(other.upper),
            width(other.width),
            pitch(other.pitch),
            items(other.items)
            {
            }

            //! show it
            friend inline std::ostream & operator<<( std::ostream &lay_os, const patch &p )
            {
                lay_os << "{ " << p.lower << " -> " << p.upper << " : width=" << p.width << " => #" << p.items << " pitch=" << p.pitch << " }";
                return lay_os;
            }

            //! offset of a coordinate in the patch
            inline unit_t offset_of( param_coord c ) const throw()
            {
                unit_t ans = unit_t(__coord::get(c,0)) - unit_t(__coord::get(lower,0));
                for( size_t i=1; i < DIM; ++i )
                    ans += unit_t(__coord::get(pitch,i)) * ( unit_t(__coord::get(c,i)) - unit_t(__coord::get(lower,i)) );
                return ans;
            }

            //!equality
            static inline bool eq(const patch &lhs, const patch &rhs) throw()
            {
                return __coord::eq(lhs.lower,rhs.lower) && __coord::eq(lhs.upper,rhs.upper);
            }

            //! has one point
            inline bool has(const_coord &C) const throw()
            {
                for(size_t i=0;i<DIM;++i)
                {
                    const_type c = __coord::get(C,i);
                    if(c<__coord::get(lower,i)) return false;
                    if(c>__coord::get(upper,i)) return false;
                }
                return true;
            }

            //! contains a sub patch
            inline bool contains(const patch &sub) const throw()
            {
                return has(sub.lower) && has(sub.upper);
            }

        private:
            YOCTO_DISABLE_ASSIGN(patch);
            

        };
    }
}

#endif

