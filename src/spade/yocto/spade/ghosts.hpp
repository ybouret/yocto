#ifndef YOCTO_SPADE_GHOSTS_INCLUDED
#define YOCTO_SPADE_GHOSTS_INCLUDED 1

#include "yocto/spade/types.hpp"

namespace yocto
{
    namespace spade
    {


        //! a ghost, ghost_info and abs size
        template <class COORD>
        class ghost_of
        {
        public:
            YOCTO_SPADE_DECL_COORD();
            const_coord peer;
            const_coord size;

            inline ghost_of(const_coord who, param_coord num) throw() :
            peer(who), size(__coord_abs<COORD>(num))
            {}

            inline ghost_of() throw() :
            peer(coord_info<coord>::zero),
            size(coord_info<coord>::zero)
            {
            }
            

            inline  ~ghost_of() throw() {}

            inline ghost_of(const ghost_of &other) throw() :
            peer(other.peer),
            size(other.size)
            {}


        private:
            YOCTO_DISABLE_ASSIGN(ghost_of);
        };


        //! a pair of ghosts with origin rank
        template <typename COORD>
        class ghosts_of
        {
        public:
            typedef ghost_of<COORD>    ghost_type;
            typedef const ghost_type   const_ghost;
            const int   rank;
            const_ghost lower;
            const_ghost upper;

            inline ghosts_of(const int   where,
                            const_ghost lo,
                            const_ghost up) throw() :
            rank(where),
            lower(lo),
            upper(up)
            {
            }

            
            inline ghosts_of(const ghosts_of &other) throw() :
            rank(other.rank),
            lower(other.lower),
            upper(other.upper)
            {
            }

        private:
            YOCTO_DISABLE_ASSIGN(ghosts_of);
        };

        typedef ghost_of<coord1D>     Ghost1D;
        typedef ghosts_of<coord1D>    Ghosts1D;

        typedef ghost_of<coord2D>     Ghost2D;
        typedef ghosts_of<coord2D>    Ghosts2D;

        typedef ghost_of<coord3D>     Ghost3D;
        typedef ghosts_of<coord3D>    Ghosts3D;

    }
}

#endif
