#ifndef YOCTO_FAME_MPI_DOMAINS_INCLUDED
#define YOCTO_FAME_MPI_DOMAINS_INCLUDED 1

#include "yocto/mpi/mpi.hpp"
#include "yocto/fame/domains.hpp"

namespace yocto
{
    namespace fame
    {

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

            //__________________________________________________________________
            //
            //! get the domain for I/O
            /**
             will lead to a full layout for the master node, and
             a dummy layout (1 item) for slave nodes.
             */
            //__________________________________________________________________
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

            // collect all domains into target
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

            //! dispatch all domains from target
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
