#include "yocto/lang/lexical/unit.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Unit:: ~Unit() throw()
            {
            }


            Unit:: Unit(const Stamp &usrStamp) throw() :
            Token(),
            next(0),
            prev(0),
            stamp(usrStamp),
            label(*stamp)
            {
            }

            Unit:: Unit(const Unit &other) :
            Token(other),
            next(0),
            prev(0),
            stamp(other.stamp),
            label(*stamp)
            {
            }
            
        }
    }
}
