#include "yocto/ipso/field.hpp"

namespace yocto
{
    namespace ipso
    {
        field_info:: ~field_info() throw()
        {
        }
        
        field_info:: field_info(const char *id) :
        name(id),
        count(0),
        bytes(0)
        {
        }



    }
}
