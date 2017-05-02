#ifndef YOCTO_SPADE_LAYOUT_INCLUDED
#define YOCTO_SPADE_LAYOUT_INCLUDED 1

#include "yocto/spade/types.hpp"
#include "yocto/type/traits.hpp"
#include <iostream>

namespace yocto
{
    namespace spade
    {

        class layout_info
        {
        public:
            const size_t dimension;
            virtual ~layout_info() throw();
            layout_info(const layout_info &) throw();

        protected:
            explicit layout_info(const size_t dim);  //!< dim = 1|2|3
            size_t setup(const void  *lower,
                         const void  *upper,
                         const void  *pitch,
                         const void  *width) throw();
        private:
            YOCTO_DISABLE_ASSIGN(layout_info);
        };

        template <typename COORD>
        class layout_of : public layout_info
        {
        public:
            typedef COORD                                         coord;         //!< POD coordinate
            typedef const     COORD                               const_coord;   //!< const coordinate
            typedef typename  type_traits<COORD>::parameter_type  param_coord;   //!< coordinate as parameter
            static const size_t DIMENSION = sizeof(COORD)/sizeof(coord1D);       //!< integral dimension

            const_coord  lower;  //!< lower coordinate(s)
            const_coord  upper;  //!< upper coordinate(s)
            const_coord  pitch;  //!< (1,nx,nx*ny)
            const_coord  width;  //!< upper-lower+1
            const size_t items;  //!< linear items in layout

            inline virtual ~layout_of() throw() {}

            //! prepare layout
            inline explicit layout_of(param_coord lower_coord, param_coord upper_coord) :
            layout_info( DIMENSION ),
            lower(lower_coord),
            upper(upper_coord),
            pitch(coord_info<COORD>::zero),
            width(coord_info<COORD>::zero), 
            items( setup(&lower,&upper,&pitch,&width) )
            {
            }

            //! copy layout_of
            inline layout_of( const layout_of &other ) throw() :
            layout_info( other ),
            lower( other.lower ),
            upper( other.upper ),
            width( other.width ),
            pitch( other.pitch ),
            items( other.items )
            {
            }

            //! get the offset
            inline coord1D offset_of( param_coord c ) const throw()
            {
                coord1D ans = __coord(c,0) - __coord(lower,0);
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

            //! comparison
            inline bool same_metrics_than( const layout_of &other ) const throw()
            {
                return are_same_coord(lower,other.lower) && are_same_coord(upper,other.upper);
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


        typedef layout_of<coord1D> layout1D;
        typedef layout_of<coord2D> layout2D;
        typedef layout_of<coord3D> layout3D;
    }
}

#endif

