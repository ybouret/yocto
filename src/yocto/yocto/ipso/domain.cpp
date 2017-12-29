#include "yocto/ipso/domain.hpp"

namespace yocto
{
    namespace ipso
    {
        metrics:: ~metrics() throw() {}

        metrics:: metrics(const size_t num_items) :
        items( num_items ),
        async( 0 ),
        local( 0 )
        {
        }

        metrics:: metrics( const metrics &other ) :
        items( other.items ),
        async( other.async ),
        local( other.local )
        {
        }
        
    }
}

