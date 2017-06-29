#ifndef YOCTO_FAME_MPI_INCLUDED
#define YOCTO_FAME_MPI_INCLUDED 1

#include "yocto/mpi/mpi.hpp"
#include "yocto/fame/ghosts-io.hpp"
#include "yocto/fame/domains.hpp"

namespace yocto
{
    namespace fame
    {
        
        //______________________________________________________________________
        //
        //
        //! memory and functions for MPI I/O
        //
        //______________________________________________________________________
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
            
            //__________________________________________________________________
            //
            //
            //! allocate IO per coordinate
            //
            //__________________________________________________________________
            inline void prepare_for(const ghosts_of<COORD> &G,
                                    const size_t            block_size )
            {
                assert(block_size>0);
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const ghosts &g = G[dim];
                    switch(g.kind)
                    {
                        case ghosts::async: {
                            const size_t num_indices = g.size();
                            const size_t num_bytes   = num_indices * block_size;
                            (*this)[dim].ensure(num_bytes);
                        } break;
                            
                        case ghosts::empty:
                            break;
                            
                        case ghosts::lcopy:
                            break;
                    }
                }
            }
            
            //__________________________________________________________________
            //
            //
            //! use field item sizes
            //
            //__________________________________________________________________
            inline void prepare_for(const ghosts_of<COORD> &G,
                                    const field_data       &fd)
            {
                this->prepare_for(G,fd.item_size);
            }
            
            
            //__________________________________________________________________
            //
            //
            //! use fields block_size
            //
            //__________________________________________________________________
            inline void prepare_for(const ghosts_of<COORD> &G, const fields &iof)
            {
                this->prepare_for(G,iof.block_size);
            }
            
            
            //__________________________________________________________________
            //
            //
            //! exchange ghosts for one field
            //
            //__________________________________________________________________
            template <typename T>
            inline void perform(const ghosts_of<COORD> &Ghosts,
                                field<T,COORD>         &F )
            {
                
                
                //______________________________________________________________
                //
                //
                //
                // first pass: local exchange
                //
                //
                //______________________________________________________________
                for(size_t i=0;i<Ghosts.num_lcopy;++i)
                {
                    const ghosts *g = Ghosts.lcopy[i];
                    ghosts_pair::swap(F,*(g->prev),*(g->next));
                }
                
                
                
                //______________________________________________________________
                //
                //
                //
                // asynchronous
                //
                //
                //______________________________________________________________
                slots_of<ghosts_io> &gIO = *this;
                MPI_Status status;
                //______________________________________________________________
                //
                // pass=0: send to prev, recv from next
                // pass=1: send to next, recv from prev
                //______________________________________________________________
                for(int pass=0;pass<2;++pass)
                {
                    for(size_t dim=0;dim<DIMENSION;++dim)
                    {
                        
                        //______________________________________________________
                        //
                        // take the ghost
                        //______________________________________________________
                        const ghosts &g    = Ghosts[dim];
                        
                        if(g.kind!=ghosts::async)
                        {
                            continue;
                        }
                        
                        //______________________________________________________
                        //
                        // take the memory
                        //______________________________________________________
                        ghosts_io    &IO    = gIO[dim]; assert(IO.capacity>=g.size()*sizeof(T));
                        
                        // analyze
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
                                        
                                    default:
                                        throw exception("corrupted code, invalid pass=%d",pass);
                                }
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
            
            //__________________________________________________________________
            //
            //
            //! exchange ghosts for multiple fields at once
            //
            //__________________________________________________________________
            inline void perform(const ghosts_of<COORD> &Ghosts,
                                fields                 &Fields )
            {
                //______________________________________________________________
                //
                //
                //
                // first pass: local exchange
                //
                //
                //______________________________________________________________
                const fields::iterator begin      = Fields.begin();
                const size_t           num_fields = Fields.size();
                
                MPI.Printf(stderr,"stream.#lcopy=%u\n", unsigned(Ghosts.num_lcopy));
                for(size_t i=0;i<Ghosts.num_lcopy;++i)
                {
                    const ghosts *g     = Ghosts.lcopy[i];
                    size_t        count = num_fields;
                    for( fields::iterator it=begin;count>0;--count,++it)
                    {
                        ghosts_pair::stream_swap(**it, *(g->prev), *(g->next) );
                    }
                }
                
                //______________________________________________________________
                //
                //
                //
                // asynchronous
                //
                //
                //______________________________________________________________
                slots_of<ghosts_io> &gIO = *this;
                MPI_Status           status;
                
                //______________________________________________________________
                //
                // pass=0: send to prev, recv from next
                // pass=1: send to next, recv from prev
                //______________________________________________________________
                for(int pass=0;pass<2;++pass)
                {
                    for(size_t dim=0;dim<DIMENSION;++dim)
                    {
                        
                        //______________________________________________________
                        //
                        // take the ghost
                        //______________________________________________________
                        const ghosts &g    = Ghosts[dim];
                        
                        if(g.kind!=ghosts::async)
                        {
                            continue;
                        }
                        
                        // take the memory
                        ghosts_io    &IO    = gIO[dim]; assert(IO.capacity>=g.size()*Fields.block_size);
                        const size_t  nxch  = g.size() * Fields.block_size;
                        
                        //______________________________________________________
                        //
                        // analyze
                        //______________________________________________________
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
                                IO.store_into_send(*sendID,Fields);
                                MPI.Sendrecv(IO.send_addr(), nxch, MPI_BYTE, sendID->rank, 0,
                                             IO.recv_addr(), nxch, MPI_BYTE, recvID->rank, 0,
                                             MPI_COMM_WORLD, status);
                                IO.query_from_recv(*recvID,Fields);
                            } break;
                                
                                //______________________________________________
                                //
                                // prev only
                                //______________________________________________
                            case ghosts::has_prev:
                            {
                                switch(pass)
                                {
                                    case 0:
                                        IO.store_into_send(*g.prev,Fields);
                                        MPI.Send(IO.send_addr(), nxch, MPI_BYTE, g.prev->rank, 0, MPI_COMM_WORLD);
                                        break;
                                        
                                    case 1:
                                        MPI.Recv(IO.recv_addr(),nxch,MPI_BYTE,g.prev->rank,0,MPI_COMM_WORLD,status);
                                        IO.query_from_recv(*g.prev,Fields);
                                        break;
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
                                    case 1:
                                        IO.store_into_send(*g.next,Fields);
                                        MPI.Send(IO.send_addr(), nxch, MPI_BYTE,g.next->rank,0,MPI_COMM_WORLD);
                                        break;
                                    case 0:
                                        MPI.Recv(IO.recv_addr(),nxch,MPI_BYTE,g.next->rank,0,MPI_COMM_WORLD,status);
                                        IO.query_from_recv(*g.next, Fields);
                                        break;
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
        
        
        //! used to collect/dispatch fields
        template <typename COORD>
        class mpi_domains : public domains<COORD>
        {
        public:
            YOCTO_FAME_DECL_COORD;
            static const int tag = 1;
            
            typedef layout<COORD> layout_type;
            
            const mpi                &MPI;

            inline virtual ~mpi_domains() throw() {}
            inline explicit mpi_domains(const mpi         &user_mpi,
                                        const_coord       *user_cpus,
                                        const layout_type &user_full,
                                        param_coord        user_pbc) :
            domains<COORD>(user_mpi.CommWorldSize,
                           user_cpus,
                           user_full,
                           user_pbc),
            MPI(user_mpi),
            cmem(this->max_items*4*sizeof(double)),
            empty()
            {
                MPI.Printf(stderr,"mpi_domains: memory=%u bytes\n", unsigned(cmem.size));
            }

            //! get the domain for I/O
            inline const domain<COORD> & io_domain() const throw()
            {
                if(MPI.IsFirst)
                {
                    return this->whole;
                }
                else
                {
                    return this->empty;
                }
            }

            template <typename T>
            inline void collect(field<T,COORD>            &target,
                                const field<T,COORD>      &source) const
            {
                assert(cmem.size>=sizeof(T)*source.inner.items);
                void                 *addr = cmem.data;
                if(MPI.IsFirst)
                {
                    // rank 0 exchange
                    source.save(source.inner,addr);
                    target.load(source.inner,addr);


                    // receive
                    MPI_Status            status;
                    const domains<COORD> &doms = *this;
                    for(size_t rank=1;rank<this->size;++rank)
                    {
                        const layout<COORD> &inner = doms[rank].inner;
                        MPI.Recv(addr,inner.items*sizeof(T), MPI_BYTE, rank, tag, MPI_COMM_WORLD, status);
                        target.load(inner,addr);
                    }

                }
                else
                {
                    source.save(source.inner,addr);
                    MPI.Send(addr,source.inner.items*sizeof(T), MPI_BYTE, 0, tag, MPI_COMM_WORLD);
                }
            }

            template <typename T>
            inline void dispatch(const field<T,COORD> &target,
                                 field<T,COORD>       &source) const
            {
                assert(cmem.size>=sizeof(T)*source.inner.items);
                void                 *addr = cmem.data;
                if(MPI.IsFirst)
                {
                    // rank 0 exchange
                    target.save(source.inner,addr);
                    source.load(source.inner,addr);

                    // send
                    const domains<COORD> &doms = *this;
                    for(size_t rank=1;rank<this->size;++rank)
                    {
                        const layout<COORD> &inner = doms[rank].inner;
                        target.save(inner,addr);
                        MPI.Send(addr,inner.items*sizeof(T), MPI_BYTE, rank, tag, MPI_COMM_WORLD);
                    }
                }
                else
                {
                    // recv
                    MPI_Status status;
                    MPI.Recv(addr,source.inner.items*sizeof(T),MPI_BYTE,0,tag,MPI_COMM_WORLD,status);
                    source.load(source.inner,addr);
                }
            }

            
            
        protected:
            cslot                     cmem;
            const empty_domain<COORD> empty;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(mpi_domains);
        };
        
        
    }
}

#endif
