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
        num_inner(n_inner),
        allocated_bytes(0)
        {
        }

        field_data:: field_data(const field_data &other) :
        name( other.name ),
        item_size(other.item_size),
        num_outer(other.num_outer),
        num_inner(other.num_inner),
        allocated_bytes(0)
        {
        }

        void  field_data:: set_allocated(const size_t bytes) throw()
        {
            (size_t &)allocated_bytes = bytes;
        }

        bool field_data:: are_compatible(const field_data &lhs, const field_data &rhs) throw()
        {
            return (lhs.num_inner==rhs.num_inner) && (rhs.num_outer==lhs.num_outer);
        }

        const string & field_data:: key() const throw()
        {
            return name;
        }

    }
}
