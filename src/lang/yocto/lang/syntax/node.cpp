#include "yocto/lang/syntax/snode.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            SNode:: ~SNode() throw()
            {
            }

            SNode:: SNode() throw() :
            next(0),
            prev(0)
            {
            }
            
        }
    }
}


