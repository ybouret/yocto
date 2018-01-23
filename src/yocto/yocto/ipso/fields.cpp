#include "yocto/ipso/fields.hpp"

namespace yocto
{
    namespace ipso
    {

        fields:: fields() throw() : fields_type()
        {
        }

        fields:: fields(const size_t n) : fields_type(n,as_capacity)
        {
        }
        
        fields:: ~fields() throw()
        {
        }

        void fields:: append( field_info &shared )
        {
            if( shared.refcount() <= 0 )
            {
                throw exception("fields: '%s' is not a pointer", * shared.name );
            }
            const field_info::pointer p( &shared );
            push_back(p);
        }

    }
}
