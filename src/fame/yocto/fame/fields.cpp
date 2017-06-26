#include "yocto/fame/fields.hpp"
#include "yocto/exceptions.hpp"

namespace yocto
{
    namespace fame
    {
        fields:: ~fields() throw()
        {
        }
        
        fields:: fields() throw() : block_size(0), fd()
        {
        }
        
        fields:: fields(const size_t n) : block_size(0), fd(n,as_capacity)
        {
        }

        
        void fields:: __record(const field_data::pointer &p)
        {
            static const char fn[] = "fields.record";

            for(iterator i=fd.begin();i!=fd.end();++i)
            {
                const field_data::pointer &q =*i;
                if( !field_data::are_compatible(*p,*q) )
                {
                    throw exception("%s: field_data are not compatible",fn);
                }
            }
            if(!fd.insert(p))
            {
                throw exception("%s: multiple fields '%s'", fn, p->key().c_str());
            }
            ( (size_t &) block_size ) += p->item_size;
        }
        
    }
}
