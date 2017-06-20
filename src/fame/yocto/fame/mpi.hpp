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
                            MPI.Printf(stderr, "mpi_ghosts@dim#%u: ASYNC: #bytes=%u/#indices=%u\n", unsigned(dim), unsigned(num_bytes), unsigned(num_indices));
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
            inline void perform( const ghosts_of<COORD> &Ghosts, field<T,COORD> &F )
            {

                //MPI.Printf0(stderr, "-- Perform Exchange\n");
                slots_of<ghosts_io> &GhostsIO = *this;

                //______________________________________________________________
                //
                //
                // first pass: local exchange
                //
                //______________________________________________________________
                //MPI.Printf0(stderr,"--\t#lcopy=%u\n", unsigned(Ghosts.num_lcopy));
                for(size_t i=0;i<Ghosts.num_lcopy;++i)
                {
                    const ghosts *g = Ghosts.lcopy[i];
                    ghosts_pair::exchange(F,*(g->prev),*(g->next));
                }



                //______________________________________________________________
                //
                //
                // asynchronous
                //
                //______________________________________________________________
                //MPI.Printf0(stderr,"--\t#async=%u\n", unsigned(Ghosts.num_async));
                MPI_Status status;
                //______________________________________________________________
                //
                // pass0: send to prev, recv from next
                // pass1: send to next, recv from prev
                //______________________________________________________________
                for(int pass=0;pass<2;++pass)
                {
                    //MPI.Printf0(stderr, "\t PASS #%d\n", pass+1);
                    for(size_t dim=0;dim<DIMENSION;++dim)
                    {

                        const ghosts &g    = Ghosts[dim];
                        //const char   *kind = g.kind_text();
                        //const char   *axID = coord_info::axis_name(dim);

                        if(g.kind!=ghosts::async)
                        {
                            //MPI.Printf(stderr,"-%s.%s\n",kind,axID);
                            continue;
                        }

                        ghosts_io    &IO    = GhostsIO[dim]; assert(IO.capacity>=g.size()*sizeof(T));
                        //MPI.Printf(stderr,"+%s.%s\n",kind,axID);

                        switch(g.flag)
                        {
                                //______________________________________________
                                //
                                // symetric case
                                //______________________________________________
                            case ghosts::has_both: {
                                const ghosts_pair *sendID = 0;
                                const ghosts_pair *recvID = 0;
                                switch(pass)
                                {
                                    case 0:
                                        sendID = g.prev;
                                        recvID = g.next;
                                        break;

                                    case 1:
                                        sendID = g.next;
                                        recvID = g.prev;
                                        break;
                                }
                                //MPI.Printf(stderr,"+%s.%s: send->%ld,recv<-%ld\n",kind,axID,sendID->rank,recvID->rank);
                                const size_t nxch = IO.load_into_send(*sendID,F);
                                MPI.Sendrecv(IO.send_addr(), nxch, MPI_BYTE, sendID->rank, 0,
                                             IO.recv_addr(), nxch, MPI_BYTE, recvID->rank, 0,
                                             MPI_COMM_WORLD, status);
                                IO.save_from_recv(*recvID,F);
                            } break;

                                //______________________________________________
                                //
                                // prev only
                                //______________________________________________
                            case ghosts::has_prev:
                            {
                                switch(pass)
                                {
                                    case 0: MPI.Send(IO.send_addr(), IO.load_into_send(*g.prev,F), MPI_BYTE, g.prev->rank, 0, MPI_COMM_WORLD); break;
                                    case 1: {
                                        const size_t nxch = g.prev->size * sizeof(T);
                                        MPI.Recv(IO.recv_addr(),nxch,MPI_BYTE,g.prev->rank,0,MPI_COMM_WORLD,status);
                                        IO.save_from_recv(*g.prev,F);
                                    } break;
                                }
                            } break;

                                //______________________________________________
                                //
                                // next only
                                //______________________________________________
                            case ghosts::has_next:
                            {
                                switch(pass)
                                {
                                    case 1: MPI.Send(IO.send_addr(), IO.load_into_send(*g.next,F), MPI_BYTE, g.next->rank, 0, MPI_COMM_WORLD); break;
                                    case 0: {
                                        const size_t nxch = g.next->size * sizeof(T);
                                        MPI.Recv(IO.recv_addr(),nxch,MPI_BYTE,g.next->rank,0,MPI_COMM_WORLD,status);
                                        IO.save_from_recv(*g.next,F);
                                    } break;
                                }
                            } break;

                            default:
                                throw exception("invalid ghosts flags!");
                        }

                    }
                }


            }
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(mpi_ghosts);
        };
        
    }
}

#endif
