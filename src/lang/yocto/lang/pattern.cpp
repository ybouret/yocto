#include "yocto/lang/pattern.hpp"

namespace yocto
{
    namespace Lang
    {
        Pattern:: ~Pattern() throw()
        {
        }

        Pattern:: Pattern( const Pattern &p ) throw() :
        uuid(p.uuid),
        next(0),
        prev(0)
        {
        }

        Pattern:: Pattern(const uint32_t t) throw() :
        uuid(t),
        next(0),
        prev(0)
        {
        }
        

    }
}
