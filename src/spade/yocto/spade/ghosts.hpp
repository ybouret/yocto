#ifndef YOCTO_SPADE_GHOSTS_INCLUDED
#define YOCTO_SPADE_GHOSTS_INCLUDED 1

#include "yocto/spade/types.hpp"

namespace yocto
{
    namespace spade
    {

        //! ghost info, to know the peer to communicate with
        class ghost_info
        {
        public:
            const int peer;

            explicit ghost_info(const int which) throw();
            virtual ~ghost_info() throw();
            ghost_info(const ghost_info &) throw();

        private:
            YOCTO_DISABLE_ASSIGN(ghost_info);
        };

        //! a ghost, ghost_info and abs size
        template <class COORD>
        class ghost : public ghost_info
        {
        public:
            YOCTO_SPADE_DECL_COORD();
            const_coord size;

            inline ghost(const int which, param_coord sz) throw() :
            ghost_info(which), size(__coord_abs<COORD>(sz))
            {}

            inline ghost(const int which) throw() :
            ghost_info(which), size( coord_info<coord>::zero )
            {
            }
            

            inline virtual ~ghost() throw() {}

            inline ghost(const ghost &other) throw() :
            ghost_info(other),
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
