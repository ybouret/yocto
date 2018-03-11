#ifndef YOCTO_IPSO_MPI_INCLUDED
#define YOCTO_IPSO_MPI_INCLUDED 1

#include "yocto/ipso/workspace.hpp"
#include "yocto/ipso/field3d.hpp"

#include "yocto/mpi/mpi.hpp"

namespace yocto
{
    namespace ipso
    {


        //______________________________________________________________________
        //
        //! common MPI operations for one dimension
        //______________________________________________________________________
        class async_ops
        {
        public:
            static const int tag = 7;
            const mpi &MPI;

            inline virtual ~async_ops() throw() {}
            inline explicit async_ops(const mpi &__MPI) throw() :
            MPI(__MPI)
            {
            }

            inline void __sendrecv(swap_buffers &iobuf,
                                   const int     send_peer,
                                   const int     recv_peer) const
            {
                MPI_Status status;
                assert(iobuf.in_recv()==iobuf.in_send());
                MPI.Sendrecv(iobuf.send_addr(), iobuf.in_send(), MPI_BYTE, send_peer, tag,
                             iobuf.recv_addr(), iobuf.in_recv(), MPI_BYTE, recv_peer, tag,
                             MPI_COMM_WORLD,status);
            }

#if 0
            inline void exchange(const swaps *swp) const
            {
                assert(swp);
                __sendrecv(swp->iobuf,swp->target,swp->target);
            }
#endif

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(async_ops);
        };

        //______________________________________________________________________
        //
        //! workspace with MPI operations
        //______________________________________________________________________
        template <typename COORD>
        class mpi_workspace : public async_ops, public workspace<COORD>
        {
        public:
            inline explicit mpi_workspace(const mpi            &__MPI,
                                          const divider<COORD> &full,
                                          const size_t          layers,
                                          const COORD           pbcs) :
            async_ops(__MPI),
            workspace<COORD>(full,MPI.CommWorldRank,layers,pbcs)
            {
            }

            //! perform synchronization on loaded buffers
            inline void synchronize()
            {
#if 0
                for(const swaps *swp=this->async.head;swp;swp=swp->next)
                {
                    exchange(swp);
                }
#endif
            }



            //! for one field of some fields, using overloaded sync_store
            template <typename FIELD_TYPE>
            inline void synchronize( FIELD_TYPE &F )
            {
                this->sync_store_begin();
                this->sync_store(F);
                this->sync_store_end();
                synchronize();
                this->sync_query(F);
            }
            
            inline virtual ~mpi_workspace() throw()
            {
            }

            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(mpi_workspace);
        };
    }
}

#endif

