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
            static const int __tag = 7;
            const mpi &MPI;

            inline virtual ~async_ops() throw() {}
            inline explicit async_ops(const mpi &__MPI) throw() :MPI(__MPI) {}

            inline void xch1d(const swaps::list &L) const
            {
                switch(L.size)
                {
                    case 0: return;

                    case 1: {
                        // only one exchange
                        const swaps &swp = *(L.head);
                        __sendrecv(swp.iobuf, swp.target,swp.iobuf,swp.target);
                    } return;

                    case 2: {
                        // two waves
                        const swaps &lower = *(L.head);
                        const swaps &upper = *(L.tail);
                        assert(lower.pos<upper.pos);
                        __sendrecv(lower.iobuf,lower.target,upper.iobuf,upper.target);
                        __sendrecv(upper.iobuf,upper.target,lower.iobuf,lower.target);
                    } return;

                    default: // shouldn't happen
                        break;
                }
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(async_ops);

            inline void __sendrecv(swap_buffers &send_buff,
                                   const int     send_peer,
                                   swap_buffers &recv_buff,
                                   const int     recv_peer) const
            {
                MPI_Status status;
                MPI.Sendrecv(send_buff.send_addr(), send_buff.in_send(), MPI_BYTE, send_peer,__tag,
                             recv_buff.recv_addr(), recv_buff.in_recv(), MPI_BYTE, recv_peer,__tag,
                             MPI_COMM_WORLD,status);
            }
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
                for(size_t dim=0;dim<YOCTO_IPSO_DIM_OF(COORD);++dim)
                {
                    xch1d(this->async[dim]);
                }
                for(size_t diag=0;diag<this->APEX;++diag)
                {
                    xch1d(this->apex_async[diag]);
                }
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

            //! destructor
            inline virtual ~mpi_workspace() throw()
            {
            }

            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(mpi_workspace);
        };
        
    }
}

#endif

