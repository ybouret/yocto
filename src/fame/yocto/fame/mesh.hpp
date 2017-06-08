#ifndef YOCTO_FAME_MESH_INCLUDED
#define YOCTO_FAME_MESH_INCLUDED 1

#include "yocto/fame/field3d.hpp"

namespace yocto
{
    namespace fame
    {

        class mesh_info : public object
        {
        public:
            const string name;

            virtual ~mesh_info() throw();
            explicit mesh_info(const string &id);
            mesh_info(const mesh_info&);

        private:
            YOCTO_DISABLE_ASSIGN(mesh_info);
        };

    }
}

#endif
