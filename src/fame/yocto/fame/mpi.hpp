#ifndef YOCTO_FAME_MPI_INCLUDED
#define YOCTO_FAME_MPI_INCLUDED 1

#include "yocto/mpi/mpi.hpp"
#include "yocto/fame/ghosts-io.hpp"

namespace yocto
{
    namespace fame
    {

        //! memory and functions for MPI I/O
        template <typename COORD>
        class mpi_ghosts : public slots_of<ghosts_io>
        {
        public:
            YOCTO_FAME_DECL_COORD;
            const mpi &MPI;

            inline virtual ~mpi_ghosts() throw() {}

            inline explicit mpi_ghosts(const mpi &usr_mpi) :
            slots_of<ghosts_io>(DIMENSION),
            MPI(usr_mpi)
            {
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    this->push_back();
                }
            }

            inline void prepare_for(const ghosts_of<COORD> &G, const size_t max_item_size )
            {
                assert(max_item_size>0);
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const ghosts &g = G[dim];
                    switch(g.kind)
                    {
                        case ghosts::async: {
                            const size_t num_indices = g.size();
                            const size_t num_bytes   = num_indices * max_item_size;
                            (*this)[dim].ensure(num_bytes);
                            MPI.Printf(stderr, "mpi_ghosts@dim#%u: ASYNC: #bytes=%u\n", unsigned(dim), unsigned(num_bytes));
                        } break;

                        case ghosts::empty:
                            MPI.Printf(stderr,"mpi_ghosts@dim#%u: EMPTY\n", unsigned(dim));
                            break;

                        case ghosts::lcopy:
                            MPI.Printf(stderr,"mpi_ghosts@dim#%u: LCOPY\n", unsigned(dim));
                            break;
                    }
                }
            }

            inline void prepare_for(const ghosts_of<COORD> &G, const field_data &fd )
            {
                this->prepare_for(G,fd.item_size);
            }


            template <typename T>
            inline void perform( const ghosts_of<COORD> &G, field<T,COORD> &F )
            {
                //______________________________________________________________
                //
                // first pass: local exchange
                //______________________________________________________________
                for(size_t i=0;i<G.num_lcopy;++i)
                {
                    const ghosts *g = G.lcopy[i];
                    ghosts_pair::exchange(F,*(g->prev),*(g->next));
                }

                //______________________________________________________________
                //
                // second pass: async exchange
                //______________________________________________________________
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const ghosts &g = G[dim];
                    if(g.kind!=ghosts::async)
                        continue;

                    ghosts_io &gio = (*this)[dim]; assert(gio.capacity>=g.size()*sizeof(T));

                    if(g.prev)
                    {
                        MPI_Status         status;
                        const ghosts_pair &gp = *(g.prev);
                        const size_t       ns = gio.load_to_send(gp,F);
                        
                        MPI.Sendrecv(gio.send_addr(), ns, MPI_BYTE, g.prev->rank, 0,
                                     gio.recv_addr(), ns, MPI_BYTE, F.self.rank, 0,
                                     MPI_COMM_WORLD, status);
                    }
                    
                }
                
            }
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(mpi_ghosts);
        };
        
    }
}

#endif
