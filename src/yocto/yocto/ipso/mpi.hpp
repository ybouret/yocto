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

            void sendrecv(const mpi             &MPI,
                          const exchange_buffer &snd,
                          const int              snd_peer,
                          exchange_buffer       &rcv,
                          const int              rcv_peer) const

            {
                MPI_Status status;
                MPI.Sendrecv(snd.addr(), snd.load(), MPI_BYTE, snd_peer, tag,
                             rcv.addr(), rcv.load(), MPI_BYTE, rcv_peer, tag, MPI_COMM_WORLD,
                             status);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(async_ops);
        };

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

            inline virtual ~mpi_workspace() throw()
            {
            }

            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(mpi_workspace);
        };

    }
}

#endif

