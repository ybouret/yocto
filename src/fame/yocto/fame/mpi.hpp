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
            inline void send(const ghosts_pair    &gp,
                             const ghosts_io      &gio,
                             const field<T,COORD> &F)
            {
                const size_t       ns = gio.load_into_send(gp,F);
                MPI.Send(gio.send_addr(),ns, MPI_BYTE, gp.rank,0,MPI_COMM_WORLD);
            }

            template <typename T>
            inline void recv(const ghosts_pair    &gp,
                             const ghosts_io      &gio,
                             field<T,COORD>       &F)
            {
                MPI_Status status;
                const size_t nr = gp.size * sizeof(T);
                MPI.Recv(gio.recv_addr(),nr,MPI_BYTE,gp.rank,0,MPI_COMM_WORLD,status);
                gio.save_from_recv(gp,F);
            }


            template <typename T>
            inline void perform( const ghosts_of<COORD> &Ghosts, field<T,COORD> &F )
            {

                MPI.Printf0(stderr, "-- Perform Exchange\n");
                slots_of<ghosts_io> &GhostsIO = *this;

                //______________________________________________________________
                //
                //
                // first pass: local exchange
                //
                //______________________________________________________________
                MPI.Printf0(stderr,"--\t#lcopy=%u\n", unsigned(Ghosts.num_lcopy));
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
                MPI.Printf0(stderr,"--\t#async=%u\n", unsigned(Ghosts.num_async));
                MPI_Status status;
                //______________________________________________________________
                //
                // pass0: send to prev, recv from next
                // pass1: send to next, recv from prev
                //______________________________________________________________

                for(int pass=0;pass<2;++pass)
                {
                    MPI.Printf0(stderr, "\t PASS #%d\n", pass+1);
                    for(size_t dim=0;dim<DIMENSION;++dim)
                    {

                        const ghosts &g    = Ghosts[dim];
                        const char   *kind = g.kind_text();
                        const char   *axID = coord_info::axis_name(dim);

                        if(g.kind!=ghosts::async)
                        {
                            MPI.Printf(stderr,"-%s.%s\n",kind,axID);
                            continue;
                        }

                        ghosts_io    &IO    = GhostsIO[dim]; assert(IO.capacity>=g.size()*sizeof(T));
                        MPI.Printf(stderr,"+%s.%s\n",kind,axID);

                        switch(g.flag)
                        {
                                // symetric case
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
                                MPI.Printf(stderr,"+%s.%s: send->%ld,recv<-%ld\n",kind,axID,sendID->rank,recvID->rank);
                                const size_t nxch = IO.load_into_send(*sendID,F);
                                MPI.Sendrecv(IO.send_addr(), nxch, MPI_BYTE, sendID->rank, 0,
                                             IO.recv_addr(), nxch, MPI_BYTE, recvID->rank, 0,
                                             MPI_COMM_WORLD, status);
                                IO.save_from_recv(*recvID,F);
                            } break;

                                // prev only
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

                                //next only
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

#if 0
                for(coord1d pass=0;pass<2;++pass)
                {
                    MPI.Printf0(stderr,"--\tPass #%d\n",int(pass));

                    for(size_t dim=0;dim<DIMENSION;++dim)
                    {

                        const ghosts &g    = Ghosts[dim];
                        const string  gk   = g.kind_text() + vformat("@%s",coord_info::axis_name(dim));
                        const char   *kind = gk.c_str();

                        if(g.kind!=ghosts::async)
                        {
                            MPI.Printf(stderr,"%s\n",kind);
                            continue;
                        }

                        const int     color = __coord(F.color,dim);
                        ghosts_io    &IO    = GhostsIO[dim]; assert(IO.capacity>=g.size()*sizeof(T));
                        const int     sz    = F.full.size;
                        if(color==pass)
                        {
                            // send
                            switch(g.flag)
                            {
                                case ghosts::has_prev: {
                                    MPI.Printf(stderr, "color=%d: send->prev=%d.%d\n",color,sz,int(g.prev->rank));
                                    send(*g.prev, IO, F);
                                } break;
                                case ghosts::has_next: {
                                    MPI.Printf(stderr, "color=%d: send->next=%d.%d\n",color,sz,int(g.next->rank));
                                    send(*g.next,IO,F);
                                } break;
                                case ghosts::has_both: {
                                    MPI.Printf(stderr, "color=%d: send->prev=%d.%d and send->next=%d.%d\n",color,sz,int(g.prev->rank),sz,int(g.next->rank));
                                    send(*g.prev,IO,F);
                                    send(*g.next,IO,F);
                                } break;
                                default:
                                    throw exception("mpi_exchange: invalid ghosts flag=%d", g.flag);
                            }
                        }
                        else
                        {
                            // recv mode
                            switch(g.flag)
                            {
                                case ghosts::has_prev: {
                                    MPI.Printf(stderr, "color=%d: recv<-prev=%d.%d\n",color,sz,int(g.prev->rank));
                                    recv(*g.prev,IO,F);
                                } break;
                                case ghosts::has_next: {
                                    MPI.Printf(stderr, "color=%d: recv<-next=%d.%d\n",color,sz,int(g.next->rank));
                                    recv(*g.next,IO,F);
                                } break;
                                case ghosts::has_both: {
                                    MPI.Printf(stderr, "color=%d: recv<-prev=%d.%d and recv<-next=%d.%d\n",color,sz,int(g.prev->rank),sz,int(g.next->rank));
                                    recv(*g.prev,IO,F);
                                    recv(*g.next,IO,F);
                                } break;
                                default:
                                    throw exception("mpi_exchange: invalid ghosts flag=%d", g.flag);
                            }
                            
                        }
                    }
                    
                }
#endif
                
            }
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(mpi_ghosts);
        };
        
    }
}

#endif
