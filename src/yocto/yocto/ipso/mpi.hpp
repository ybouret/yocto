#ifndef YOCTO_IPSO_MPI_INCLUDED
#define YOCTO_IPSO_MPI_INCLUDED 1

#include "yocto/ipso/workspace.hpp"
#include "yocto/ipso/partition.hpp"
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

            virtual ~async_ops() throw() {}
            explicit async_ops(const mpi &__MPI) throw() :
            MPI(__MPI)
            {
            }

            //! wraps a send/recv operation
            inline void __sendrecv(const mpi             &MPI,
                                   const exchange_buffer &snd,
                                   const int              snd_peer,
                                   exchange_buffer       &rcv,
                                   const int              rcv_peer) const

            {
                MPI_Status status;
                MPI.Sendrecv(snd.addr(), snd.load(), MPI_BYTE, snd_peer, tag,
                             rcv.addr(), rcv.load(), MPI_BYTE, rcv_peer, tag,
                             MPI_COMM_WORLD,status);
            }


            //! perform MPI data exchange
            inline void __exchange1D(const ghosts::list     &gl,
                                     exchange_buffers::list &bl)
            {
                assert(gl.size==bl.size);
                switch(gl.size)
                {
                    case 1: {
                        const ghosts     &g = *(gl.head);
                        exchange_buffers &b = *(bl.head);
                        assert( b.send.load() == b.recv.load() );
                        __sendrecv(MPI,b.send,g.target,b.recv,g.target);
                    } break;

                    case 2:{
                        const ghosts     &g_lo = *(gl.head);
                        const ghosts     &g_up = *(gl.tail);
                        exchange_buffers &b_lo = *(bl.head);
                        exchange_buffers &b_up = *(bl.tail);
                        // up to lo
                        __sendrecv(MPI,b_up.send,g_up.target,b_lo.recv,g_lo.target);
                        // lo to up
                        __sendrecv(MPI,b_lo.send,g_lo.target,b_up.recv,g_up.target);
                    } break;

                    default:
                        break;
                }
            }

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
                                          coord1D               ng,
                                          const COORD           pbcs,
                                          const size_t          block_size) :
            async_ops(__MPI),
            workspace<COORD>(full,MPI.CommWorldRank,ng,pbcs,block_size)
            {
            }

            //! perform synchronization on loaded buffers
            inline void synchronize()
            {
                for(size_t dim=0;dim<workspace<COORD>::DIM;++dim)
                {
                    __exchange1D(this->async[dim],this->iobuf[dim]);
                }
            }

            //! for one field of some fields
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

