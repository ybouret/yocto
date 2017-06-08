#ifndef YOCTO_FAME_VISIT_INCLUDED
#define YOCTO_FAME_VISIT_INCLUDED 1

#include "yocto/visit/interface.hpp"
#include "yocto/fame/mesh/rectilinear.hpp"

namespace yocto
{

    namespace fmae
    {

        struct __visit
        {

            template <typename QUAD_MESH>
            static inline
            void QuadMeshMetaData(visit_handle    &m,
                                  const QUAD_MESH &mesh)
            {

            }
            
        };

    }

}



#endif

