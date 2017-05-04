#ifndef YOCTO_SPADE_GHOSTS_INCLUDED
#define YOCTO_SPADE_GHOSTS_INCLUDED 1

#include "yocto/spade/types.hpp"

namespace yocto
{
    namespace spade
    {


        //! a ghost, ghost_info and abs size
        template <class COORD>
        class Ghost
        {
        public:
            YOCTO_SPADE_DECL_COORD();
            const_coord peer;
            const_coord size;

            inline Ghost(const_coord who, param_coord num) throw() :
            peer(who), size(__coord_abs<COORD>(num))
            {}

            inline Ghost() throw() :
            peer(coord_info<coord>::zero),
            size(coord_info<coord>::zero)
            {
            }
            

            inline virtual ~Ghost() throw() {}

            inline Ghost(const Ghost &other) throw() :
            peer(other.peer),
            size(other.size)
            {}


        private:
            YOCTO_DISABLE_ASSIGN(Ghost);
        };


        //! a pair of ghosts with origin rank
        template <typename COORD>
        class Ghosts : public object
        {
        public:
            typedef Ghost<COORD>     ghost_type;
            typedef const ghost_type const_ghost;
            const int   rank;
            const_ghost lower;
            const_ghost upper;

            inline Ghosts(const int   where,
                          const_ghost lo,
                          const_ghost up) throw() :
            rank(where),
            lower(lo),
            upper(up)
            {
            }

            
            inline Ghosts(const Ghosts &other) throw() :
            rank(other.rank),
            lower(other.lower),
            upper(other.upper)
            {
            }

        private:
            YOCTO_DISABLE_ASSIGN(Ghosts);
        };

        typedef Ghost<coord1D>  Ghost1D;
        typedef Ghosts<coord1D> Ghosts1D;

        typedef Ghost<coord2D>  Ghost2D;
        typedef Ghosts<coord2D> Ghosts2D;

        typedef Ghost<coord3D>  Ghost3D;
        typedef Ghosts<coord3D> Ghosts3D;


     
        
    }
}

#endif
