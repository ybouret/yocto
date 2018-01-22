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
        template <typename COORD>
        class mpi_workspace : public workspace<COORD>
        {
        public:
            inline explicit mpi_workspace(const divider<COORD> &full,
                                          const size_t          rank,
                                          coord1D               ng,
                                          const COORD           pbcs,
                                          const size_t          block_size) :
            workspace<COORD>(full,rank,ng,pbcs,block_size)
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

