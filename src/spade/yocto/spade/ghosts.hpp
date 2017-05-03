#ifndef YOCTO_SPADE_GHOSTS_INCLUDED
#define YOCTO_SPADE_GHOSTS_INCLUDED 1

#include "yocto/spade/types.hpp"
#include "yocto/sequence/vector.hpp"

namespace yocto
{
    namespace spade
    {


        //! a ghost, ghost_info and abs size
        template <class COORD>
        class ghost
        {
        public:
            YOCTO_SPADE_DECL_COORD();
            const_coord peer;
            const_coord size;

            inline ghost(const_coord who, param_coord num) throw() :
            peer(who), size(__coord_abs<COORD>(num))
            {}

            inline ghost() throw() :
            peer(coord_info<coord>::zero),
            size(coord_info<coord>::zero)
            {
            }
            

            inline virtual ~ghost() throw() {}

            inline ghost(const ghost &other) throw() :
            peer(other.peer),
            size(other.size)
            {}


        private:
            YOCTO_DISABLE_ASSIGN(ghost);
        };


        //! a pair of ghosts with origin rank
        template <typename COORD>
        class ghosts : public object
        {
        public:
            typedef ghost<COORD>     ghost_type;
            typedef const ghost_type const_ghost;
            const int   rank;
            const_ghost lower;
            const_ghost upper;

            inline ghosts(const int   where,
                          const_ghost lo,
                          const_ghost up) throw() :
            rank(where),
            lower(lo),
            upper(up)
            {
            }

            
            inline ghosts(const ghosts &other) throw() :
            rank(other.rank),
            lower(other.lower),
            upper(other.upper)
            {
            }

        private:
            YOCTO_DISABLE_ASSIGN(ghosts);
        };

        typedef ghost<coord1D>  ghost1D;
        typedef ghosts<coord1D> ghosts1D;

        typedef ghost<coord2D>  ghost2D;
        typedef ghosts<coord2D> ghosts2D;

        typedef ghost<coord3D>  ghost3D;
        typedef ghosts<coord3D> ghosts3D;


     
        
    }
}

#endif
