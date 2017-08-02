#include "yocto/lang/lexeme.hpp"

namespace yocto
{
    namespace Lang
    {
        Lexeme:: Lexeme(const string &usrLabel) throw() :
        Token(),
        label(usrLabel),
        next(0),
        prev(0)
        {
        }

        Lexeme:: ~Lexeme() throw()
        {
        }
        
    }
}
