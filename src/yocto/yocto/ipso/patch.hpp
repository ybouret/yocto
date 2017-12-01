#ifndef YOCTO_IPSO_PATCH_INCLUDED
#define YOCTO_IPSO_PATCH_INCLUDED 1

#include "yocto/ipso/coord.hpp"
#include "yocto/code/bswap.hpp"
#include <iostream>

namespace yocto
{
    namespace ipso
    {

        template <typename COORD>
        class patch
        {
        public:
            static const size_t DIM = sizeof(COORD)/sizeof(coord1D);

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
                coord1D   *l =  (coord1D   *)&lower;
                coord1D   *u =  (coord1D   *)&upper;
                coord1D   *w =  (coord1D   *)&width;
                size_t &n = *(size_t *)&items;
                for(size_t i=0;i<DIM;++i)
                {
                    if(l[i]>u[i]) cswap(l[i],u[i]);
                    n *= size_t( w[i] = coord1D(1)+u[i]-l[i] );
                }
                coord1D *p = (coord1D *)&pitch;
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
                unit_t ans = unit_t(__coord(c,0)) - unit_t(__coord(lower,0));
                for( size_t i=1; i < DIM; ++i )
                    ans += unit_t(__coord(pitch,i)) * ( unit_t(__coord(c,i)) - unit_t(__coord(lower,i)) );
                return ans;
            }

            //!equality
            static inline bool eq(const patch &lhs, const patch &rhs) throw()
            {
                return __coord_eq(lhs.lower,rhs.lower) && __coord_eq(lhs.upper,rhs.upper);
            }

            //! has one point
            inline bool has(const_coord &C) const throw()
            {
                for(size_t i=0;i<DIM;++i)
                {
                    const coord1D c = __coord(C,i);
                    if(c<__coord(lower,i)) return false;
                    if(c>__coord(upper,i)) return false;
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

