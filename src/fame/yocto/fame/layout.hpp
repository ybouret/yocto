#ifndef YOCTO_FAME_LAYOUT_INCLUDED
#define YOCTO_FAME_LAYOUT_INCLUDED 1

#include "yocto/fame/types.hpp"
#include <iostream>

namespace yocto
{
    namespace fame
    {
        
        template <typename COORD>
        class layout_of
        {
        public:
            YOCTO_FAME_DECL_COORD;
            
            const_coord  lower;  //!< lower coordinate(s)
            const_coord  upper;  //!< upper coordinate(s)
            const_coord  pitch;  //!< (1,nx,nx*ny)
            const_coord  width;  //!< upper-lower+1
            const size_t items;  //!< linear items in layout

            inline virtual ~layout_of() throw() {}
            inline explicit layout_of(param_coord L,param_coord U) throw() :
            lower(L),
            upper(U),
            pitch( coord_data<coord>::zero ),
            width( coord_data<coord>::zero ),
            items(1)
            {
                
                coord1d *w = __coord_addr(width);
                {
                    coord1d *l = __coord_addr(lower);
                    coord1d *u = __coord_addr(upper);
                    size_t  &n = (size_t&)items;
                    for(size_t dim=0;dim<DIMENSION;++dim)
                    {
                        if(l[dim]>u[dim]) cswap(l[dim],u[dim]);
                        n *= ( w[dim]=1+(u[dim]-l[dim]));
                    }
                }
                
                {
                    coord1d *p = __coord_addr(pitch);
                    p[0] = 1;
                    for(size_t dim=1;dim<DIMENSION;++dim)
                    {
                        p[dim] = p[dim-1] * w[dim-1];
                    }
                }
                
            }
            
            inline layout_of(const layout_of &other) throw() :
            lower(other.lower),
            upper(other.upper),
            pitch(other.pitch),
            width(other.width),
            items(other.items)
            {
                
            }
            
            //! get the offset
            inline coord1d offset_of( param_coord c ) const throw()
            {
                coord1d ans = __coord(c,0) - __coord(lower,0);
                for( size_t i=1; i < DIMENSION; ++i )
                    ans += __coord(pitch,i) * ( __coord(c,i) - __coord(lower,i) );
                return ans;
            }
            
            //! show it
            friend inline std::ostream & operator<<( std::ostream &lay_os, const layout_of &p )
            {
                lay_os << "{ " << p.lower << " -> " << p.upper << " : width=" << p.width << " => #" << p.items << " pitch=" << p.pitch << " }";
                return lay_os;
            }

            inline bool equals( const layout_of &other ) const throw()
            {
                return are_same_coord(lower,other.lower) && are_same_coord(upper,other.upper);
            }
            
            friend inline
            bool operator==(const layout_of &lhs, const layout_of &rhs) throw()
            {
                return lhs.equals(rhs);
            }
            
            //! test coordinate inside layout
            inline bool has( param_coord C ) const throw() { return is_coord_inside(C,lower,upper); }
            
            //! test sub layout inside this layout
            inline bool contains( const layout_of &sub ) const throw()
            {
                return has(sub.lower) && has(sub.upper);
            }

            
        private:
            YOCTO_DISABLE_ASSIGN(layout_of);
        };
        
    }
}

#endif
