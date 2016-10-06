#include "yocto/container/manager.hpp"

namespace yocto
{
    container_manager::container_manager() throw() {}
    container_manager::~container_manager() throw() {}

    bool container_manager:: belongs_to(const size_t i, const group_t group_id ) throw()
    {
        assert(group_id>0);
        const group_t group_bit = (default_group << i);
        return 0 != (group_bit&group_id);
    }

    container_manager::group_t container_manager::check_group(group_t group_id) throw()
    {
        if(group_id<=0)
        {
            return default_group;
        }
        else
        {
            return group_id;
        }
    }
}
