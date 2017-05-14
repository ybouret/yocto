#ifndef YOCTO_MPI_SPLIT_INCLUDED
#define YOCTO_MPI_SPLIT_INCLUDED 1

#include "yocto/math/point3d.hpp"

namespace yocto
{
    struct mpi_split
    {
        template <typename T> static inline
        void in1D(const int rank,
                  const int size,
                  T        &offset,
                  T        &length) throw()
        {
            assert(size>0);
            assert(rank<size);
            T todo(length/size);
            for(int r=1;r<=rank;++r)
            {
                length -= todo;
                offset += todo;
                todo    = length/(size-r);
            }
            length = todo;
        }

        //! rank =
        template <typename T> static inline
        point2d<T> local_ranks(const int         rank,
                               const int         size,
                               const point2d<T> &sizes) throw()
        {
            return point2d<T>();
        }


    };
}

#endif
