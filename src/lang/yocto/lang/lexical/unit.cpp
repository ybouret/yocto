#include "yocto/lang/lexical/unit.hpp"

namespace yocto
{
    namespace Lang
    {
        Unit:: ~Unit() throw()
        {
        }


        Unit:: Unit(const Stamp &usrStamp) throw() :
        Token(),
        stamp(usrStamp),
        label(*stamp)
        {
        }

        Unit:: Unit(const Unit &other) :
        Token(other),
        stamp(other.stamp),
        label(*stamp)
        {
        }

    }
}
