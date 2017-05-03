#include "yocto/spade/field.hpp"

namespace yocto
{
    namespace spade
    {

        field_info:: ~field_info() throw() {}

        field_info:: field_info(const size_t dim) throw() :
        dimension(dim),
        bytes(0)
        {
        }

        void field_info:: set_bytes(const size_t n) throw()
        {
            (size_t &)bytes = n;
        }
    }
}
