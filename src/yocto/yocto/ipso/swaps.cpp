#include "yocto/ipso/swaps.hpp"

namespace yocto
{
    namespace ipso
    {


        swaps:: ~swaps() throw()
        {
        }

        swaps:: swaps(const size_t source_rank,
                      const size_t target_rank,
                      const size_t some_layers) throw() :
        send(),
        recv(),
        count(0),
        source(source_rank),
        target(target_rank),
        layers(some_layers),
        next(0),
        prev(0)
        {
            assert(layers>0);
        }
        
    }
}
