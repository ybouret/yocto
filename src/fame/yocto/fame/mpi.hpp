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
            inline void send(const ghosts_pair    &gp,
                             const ghosts_io      &gio,
                             const field<T,COORD> &F)
            {
                const size_t       ns = gio.load_into_send(gp,F);
                MPI.Send(gio.send_addr(),ns, MPI_BYTE, gp.rank, 0, MPI_COMM_WORLD);
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

                MPI.Printf0(stderr, "-- perform exchange --\n");
                slots_of<ghosts_io> &GhostsIO = *this;

                //______________________________________________________________
                //
                // first pass: local exchange
                //______________________________________________________________
                for(size_t i=0;i<Ghosts.num_lcopy;++i)
                {
                    const ghosts *g = Ghosts.lcopy[i];
                    ghosts_pair::exchange(F,*(g->prev),*(g->next));
                }

                //______________________________________________________________
                //
                // async exchange 1/2
                //______________________________________________________________
                for(size_t dim=0;dim<DIMENSION;++dim)
                {

                    const ghosts &g = Ghosts[dim];
                    MPI.Printf(stderr,"is %s\n",g.kind_text());
                    if(g.kind!=ghosts::async)
                    {
                        continue;
                    }

                    const int     color = __coord(F.color,dim);
                    ghosts_io    &IO    = GhostsIO[dim]; assert(IO.capacity>=g.size()*sizeof(T));

                    if(color)
                    {
                        if(g.prev) {  MPI.Printf(stderr,"color=%d: recv@prev=%ld\n", color, g.prev->rank); recv(*g.prev,IO,F); } else { MPI.Printf(stderr,"no prev\n"); }
                        if(g.next) {  MPI.Printf(stderr,"color=%d: recv@next=%ld\n", color, g.next->rank); recv(*g.next,IO,F); } else { MPI.Printf(stderr,"no next\n"); }
                    }
                    else
                    {
                        if(g.next) {  MPI.Printf(stderr, "color=%d: send@next=%ld\n", color, g.next->rank); send(*g.next,IO,F); } else { MPI.Printf(stderr,"no next\n"); }
                        if(g.prev) {  MPI.Printf(stderr, "color=%d: send@prev=%ld\n", color, g.prev->rank); send(*g.prev,IO,F); } else { MPI.Printf(stderr,"no prev\n"); }
                    }
                }

                //______________________________________________________________
                //
                // async exchange 2/2
                //______________________________________________________________
                for(size_t dim=0;dim<DIMENSION;++dim)
                {

                    const ghosts &g = Ghosts[dim];
                    MPI.Printf(stderr,"is %s\n",g.kind_text());
                    if(g.kind!=ghosts::async)
                    {
                        continue;
                    }

                    const int     color = __coord(F.color,dim);
                    ghosts_io    &IO    = GhostsIO[dim];

                    if(color)
                    {
                        if(g.next) {  MPI.Printf(stderr, "color=%d: send@next=%ld\n", color, g.next->rank); send(*g.next,IO,F); } else { MPI.Printf(stderr,"no next\n"); }
                        if(g.prev) {  MPI.Printf(stderr, "color=%d: send@prev=%ld\n", color, g.prev->rank); send(*g.prev,IO,F); } else { MPI.Printf(stderr,"no prev\n"); }
                    }
                    else
                    {
                        if(g.prev) {  MPI.Printf(stderr,"color=%d: recv@prev=%ld\n", color, g.prev->rank); recv(*g.prev,IO,F); } else { MPI.Printf(stderr,"no prev\n"); }
                        if(g.next) {  MPI.Printf(stderr,"color=%d: recv@next=%ld\n", color, g.next->rank); recv(*g.next,IO,F); } else { MPI.Printf(stderr,"no next\n"); }
                    }


                }


            }
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(mpi_ghosts);
        };
        
    }
}

#endif
