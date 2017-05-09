#include "yocto/spade/mesh.hpp"

namespace yocto
{
    namespace spade
    {
        Mesh::~Mesh() throw()
        {
        }


        Mesh:: Mesh(const string &id, const size_t dim) :
        name(id),
        dimension(dim)
        {
        }
        
    }
}
