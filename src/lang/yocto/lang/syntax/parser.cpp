#include "yocto/lang/syntax/parser.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            Parser:: ~Parser() throw()
            {
            }

            Parser:: Parser( const string &id ) :
            Grammar(id+".grammar"),
            Lexer(id+".lexer","root")
            {
            }
            
        }

    }

}
