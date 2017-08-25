#include "yocto/lang/syntax/node.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            Node:: ~Node() throw()
            {
            }

            Node:: Node(const Rule &r) :
            next(0),
            prev(0),
            parent(0),
            origin(&r),
            terminal(false)
            {
            }
            
        }
    }
}


