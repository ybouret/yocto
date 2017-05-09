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
            YOCTO_SPADE_DECL_COORD;
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

            inline friend
            bool operator==(const ghost_of &lhs, const ghost_of &rhs) throw()
            {
                return (lhs.peer == rhs.peer) && (lhs.size == rhs.size);
            }

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

            inline ghosts_of(const int  where,
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

            inline friend
            bool operator==(const ghosts_of &lhs, const ghosts_of &rhs)
            {
                return (lhs.rank == rhs.rank) && (lhs.lower==rhs.lower) && (lhs.upper==rhs.upper);
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


        struct ghost_ops
        {
            template <typename GHOST> static inline
            Ghost1D extract(const GHOST &G, const size_t dim) throw()
            {
                const coord1D peer = __coord(G.peer,dim);
                const coord1D size = __coord(G.size,dim);
                return Ghost1D(peer,size);
            }

            //! take xy components
            static inline
            Ghost2D extract(const Ghost3D &G) throw()
            {
                const coord2D peer = coord2D(G.peer.x,G.peer.y);
                const coord2D size = coord2D(G.size.x,G.size.y);
                return Ghost2D(peer,size);
            }

        };

        struct ghosts_ops
        {
            template <typename GHOSTS> static inline
            Ghosts1D extract(const GHOSTS &G,const size_t dim) throw()
            {
                const int     rank = G.rank;
                const Ghost1D g_lo = ghost_ops::extract(G.lower,dim);
                const Ghost1D g_up = ghost_ops::extract(G.upper,dim);
                
                return Ghosts1D(rank,g_lo,g_up);
            }

            static inline
            Ghosts2D extract(const Ghosts3D &G) throw()
            {
                const int      rank = G.rank;
                const Ghost2D  g_lo = ghost_ops::extract(G.lower);
                const Ghost2D  g_up = ghost_ops::extract(G.upper);

                return Ghosts2D(rank,g_lo,g_up);
            }


        };


    }
}

#endif
