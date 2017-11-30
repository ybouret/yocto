#ifndef YOCTO_IPSO_UTILS_INCLUDED
#define YOCTO_IPSO_UTILS_INCLUDED 1

#include "yocto/os.hpp"
#include <cmath>


namespace yocto
{
    namespace ipso
    {
        template <typename U, typename T> inline
        void basic_split(const U rank,
                         const U size,
                         T      &offset,
                         T      &length) throw()
        {
            assert(size>0);
            assert(rank<size);
            T todo(length/size);
            for(U r=1;r<=rank;++r)
            {
                length -= todo;
                offset += todo;
                todo    = length/(size-r);
            }
            length = todo;
        }

        inline
        double efficiency(const double speed_up,
                          const size_t nthreads)
        {
            if(nthreads<=1)
            {
                return 100.0;
            }
            else
            {
                return floor(10000.0*(speed_up-1.0)/(nthreads-1))/100.0;
            }
        }
    }

}

#endif

