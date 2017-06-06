#include "yocto/fame/field.hpp"

namespace yocto
{
    namespace fame
    {
        field_data:: ~field_data() throw()
        {
        }

        field_data:: field_data(const string &id,
                                const size_t  b_item,
                                const size_t  n_outer,
                                const size_t  n_inner) :
        name(id),
        item_size(b_item),
        num_outer(n_outer),
        num_inner(n_inner)
        {
        }

        field_data:: field_data(const field_data &other) :
        name( other.name ),
        item_size(other.item_size),
        num_outer(other.num_outer),
        num_inner(other.num_inner)
        {
        }



    }
}
