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
                throw exception("fields: '%s' is not shared", * shared.name );
            }
            const field_info::pointer p( &shared );
            push_back(p);
        }

        size_t  fields:: block_size() const throw()
        {
            size_t ans = 0;
            for(size_t i=size();i>0;--i)
            {
                ans += (*this)[i]->item_size;
            }
            return ans;
        }


    }
}
