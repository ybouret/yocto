#ifndef YOCTO_FAME_MESH_CURVILINEAR_INCLUDED
#define YOCTO_FAME_MESH_CURVILINEAR_INCLUDED 1

#include "yocto/fame/mesh.hpp"
#include "yocto/fame/layouts.hpp"

namespace yocto
{
    namespace fame
    {
        template <typename T,typename COORD>
        class curvilinear_mesh :
        public mesh_info
        {
        public:
            virtual ~curvilinear_mesh() throw() {}
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(curvilinear_mesh);
        };

    }

}

#endif
