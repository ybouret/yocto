#ifndef YOCTO_FAME_MPI_INCLUDED
#define YOCTO_FAME_MPI_INCLUDED 1

#include "yocto/mpi/mpi.hpp"
#include "yocto/fame/ghosts.hpp"
#include "yocto/container/cslot.hpp"

namespace yocto
{
    namespace fame
    {

        struct __MPI
        {
            template <typename T,typename COORD> static inline
            size_t BytesToExchange( const ghosts_of<COORD> &G, const field<T,COORD> &F)
            {
                size_t xbytes = 0;

                for(size_t dim=0;dim<YOCTO_FAME_DIM_OF(COORD);++dim)
                {
                    const ghosts &gs = G[dim];
                    if(ghosts::async==gs.kind)
                    {
                        const size_t ng = gs.size();
                        const size_t nb = ng * sizeof(T);
                        if(nb>xbytes) xbytes=nb;
                    }
                }

                return xbytes;
            }

            template <typename T,typename COORD> static inline
            void Exchange(const mpi              &MPI,
                          const ghosts_of<COORD> &G,
                          field<T,COORD>         &F,
                          cslot                  &cmem )
            {
                assert(cmem.size>=BytesToExchange(G,F));

                //______________________________________________________________
                //
                // first pass: exchange local copy
                //______________________________________________________________
                MPI.Printf0(stderr,"Exchange: LCOPY\n");
                for(size_t i=0;i<G.num_lcopy;++i)
                {
                    const ghosts &gs = *G.lcopy[i];
                    ghosts_pair::exchange(F,*gs.prev,*gs.next);
                }

            }

        };

    }
}

#endif
