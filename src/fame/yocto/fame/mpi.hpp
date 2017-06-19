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
                // first pass: local exchange
                //______________________________________________________________
                MPI.Printf0(stderr,"--\t#lcopy=%u\n", unsigned(Ghosts.num_lcopy));
                for(size_t i=0;i<Ghosts.num_lcopy;++i)
                {
                    const ghosts *g = Ghosts.lcopy[i];
                    ghosts_pair::exchange(F,*(g->prev),*(g->next));
                }

                for(coord1d pass=0;pass<2;++pass)
                {
                    MPI.Printf0(stderr,"\n--\tPass #%d\n",int(pass));

                    for(size_t dim=0;dim<DIMENSION;++dim)
                    {

                        // topology information
                        const int     color  = __coord(F.color,dim);
                        const bool    is_odd = 0 != ( __coord(F.full.sizes,dim) & 0x01 );
                        const int     l_rank = __coord(F.self.ranks,dim);
                        const int     l_rmax = __coord(F.full.rmax,dim);
                        const bool    special_head = is_odd && (l_rank<=0);
                        const bool    special_tail = is_odd && (l_rank>=l_rmax);
                        const int     sz     = F.full.size;

                        const ghosts &g      = Ghosts[dim];
                        const string  gk     = g.kind_text() + vformat("@%s",coord_info::axis_name(dim));
                        const char   *kind   = gk.c_str();

                        if(g.kind!=ghosts::async)
                        {
                            MPI.Printf(stderr,"%s\n",kind);
                            continue;
                        }
                        else
                        {
                            ghosts_io    &IO    = GhostsIO[dim]; assert(IO.capacity>=g.size()*sizeof(T));
                            if(color==pass)
                            {
                                //______________________________________________
                                //
                                // send mode, plus special receives
                                //______________________________________________
                                switch(g.flag)
                                {
                                        //______________________________________
                                        //
                                        // prev only => not cyclic
                                        //______________________________________
                                    case ghosts::has_prev: {
                                        MPI.Printf(stderr, "color=%d: send->prev=%d.%d\n",color,sz,int(g.prev->rank));
                                        send(*g.prev, IO, F);
                                    } break;

                                        //______________________________________
                                        //
                                        // next only => not cyclic
                                        //______________________________________
                                    case ghosts::has_next: {
                                        MPI.Printf(stderr, "color=%d: send->next=%d.%d\n",color,sz,int(g.next->rank));
                                        send(*g.next,IO,F);
                                    } break;

                                        //______________________________________
                                        //
                                        // both: special receives
                                        //______________________________________
                                    case ghosts::has_both: {
                                        MPI.Printf(stderr, "color=%d: send->prev=%d.%d and send->next=%d.%d\n",color,sz,int(g.prev->rank),sz,int(g.next->rank));

                                        if(special_tail)
                                        {
                                            recv(*g.next,IO,F);
                                        }
                                        send(*g.prev,IO,F);

                                        if(special_head)
                                        {
                                            recv(*g.prev,IO,F);
                                        }
                                        send(*g.next,IO,F);

                                    } break;
                                    default:
                                        throw exception("mpi_exchange: invalid ghosts flag=%d", g.flag);
                                }
                            }
                            else
                            {

                                //______________________________________________
                                //
                                // recv mode, plus special sends
                                //______________________________________________
                                switch(g.flag)
                                {
                                        //______________________________________
                                        //
                                        // prev only => not cyclic
                                        //______________________________________
                                    case ghosts::has_prev: {
                                        MPI.Printf(stderr, "color=%d: recv<-prev=%d.%d\n",color,sz,int(g.prev->rank));
                                        recv(*g.prev,IO,F);
                                    } break;

                                        //______________________________________
                                        //
                                        // next only => not cyclic
                                        //______________________________________
                                    case ghosts::has_next: {
                                        MPI.Printf(stderr, "color=%d: recv<-next=%d.%d\n",color,sz,int(g.next->rank));
                                        recv(*g.next,IO,F);
                                    } break;

                                        //______________________________________
                                        //
                                        // both: special sends
                                        //______________________________________
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
