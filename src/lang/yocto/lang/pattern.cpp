#include "yocto/lang/pattern.hpp"

namespace yocto
{
    namespace Lang
    {
        pattern:: ~pattern() throw()
        {
        }

        pattern:: pattern( const pattern &p ) throw() :
        uuid(p.uuid)
        {
        }

        pattern:: pattern(const uint32_t t) throw() :
        uuid(t)
        {
        }
        

    }
}
