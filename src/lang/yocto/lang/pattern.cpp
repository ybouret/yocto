#include "yocto/lang/pattern.hpp"

namespace yocto
{
    namespace Lang
    {
        Pattern:: ~Pattern() throw()
        {
        }

        Pattern:: Pattern( const Pattern &p ) throw() :
        uuid(p.uuid)
        {
        }

        Pattern:: Pattern(const uint32_t t) throw() :
        uuid(t)
        {
        }
        

    }
}
