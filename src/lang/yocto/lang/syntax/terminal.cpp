#include "yocto/lang/syntax/terminal.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            Terminal:: ~Terminal() throw()
            {

            }

            Terminal:: Terminal(const string &id) :
            Rule(id,UUID)
            {
            }

            

        }

    }
    
}
