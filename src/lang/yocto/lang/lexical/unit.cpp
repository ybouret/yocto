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


            Unit:: Unit(const string &labelRef) throw() :
            Token(),
            next(0),
            prev(0),
            label(labelRef)
            {
            }

            Unit:: Unit(const Unit &other) :
            Token(other),
            next(0),
            prev(0),
            label(other.label)
            {
            }
            
        }
    }
}
