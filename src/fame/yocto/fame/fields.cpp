#include "yocto/fame/fields.hpp"

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
        
        size_t fields:: size() const throw()
        {
            return fd.size();
        }
        
        void fields:: add( const field_data::pointer &p)
        {
            fd.push_back(p);
            ( (size_t &) block_size ) += p->item_size;
        }

        
    }
}
