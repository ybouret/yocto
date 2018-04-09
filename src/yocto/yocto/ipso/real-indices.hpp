#ifndef YOCTO_IPSO_REAL_INDICES_INCLUDED
#define YOCTO_IPSO_REAL_INDICES_INCLUDED 1

#include "yocto/math/point3d.hpp"

namespace yocto {

    namespace ipso
    {
        class real_indices
        {
        public:
            typedef point3d<int> indx_t;
            indx_t imin;
            indx_t imax;

            real_indices() throw();
            ~real_indices() throw();
            real_indices( const real_indices &other ) throw();
            real_indices & operator=(const real_indices &other) throw();

        };
    }
}
#endif

