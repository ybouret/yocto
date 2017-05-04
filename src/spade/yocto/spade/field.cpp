#include "yocto/spade/field.hpp"

namespace yocto
{
    namespace spade
    {

        field_info:: ~field_info() throw() {}

        field_info:: field_info(const string &id,
                                const size_t dim) throw() :
        name(id),
        dimension(dim),
        allocated(0)
        {
        }

        void field_info:: set_allocated(const size_t n) throw()
        {
            (size_t &)allocated = n;
        }
    }
}
