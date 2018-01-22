#include "yocto/ipso/metrics.hpp"

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

        int metrics::compare(const metrics &lhs, const metrics &rhs) throw()
        {
            if(lhs.items<rhs.items)
            {
                return -1;
            }
            else
            {
                if(rhs.items<lhs.items)
                {
                    return 1;
                }
                else
                {
                    assert(lhs.items==rhs.items);
                    if(lhs.async<rhs.async)
                    {
                        return -1;
                    }
                    else
                    {
                        if(rhs.async<lhs.async)
                        {
                            return 1;
                        }
                        else
                        {
                            assert(lhs.async==rhs.async);
                            return type::compare(lhs.local,rhs.local);
                        }
                    }
                }
            }
        }


    }
}

