#include "yocto/fame/mesh.hpp"

namespace yocto
{
    namespace fame
    {
        mesh_info:: ~mesh_info() throw()
        {
        }

        mesh_info:: mesh_info(const string &id) :
        name(id)
        {
            //if(dim<1||dim>3) throw exception("fame.mesh: invalid dimension=%u", unsigned(dimension));
        }

        mesh_info:: mesh_info(const mesh_info &other) :
        name(other.name)
        {
        }

    }

}
