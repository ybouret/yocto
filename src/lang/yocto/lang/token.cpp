#include "yocto/lang/token.hpp"
#include <iostream>

namespace yocto
{
    namespace Lang
    {
        Token:: ~Token() throw()
        {
        }

        Token:: Token() throw() : Chars() {}


        Token:: Token(const Token &other) :
        Chars(other)
        {

        }


        std::ostream & operator<<( std::ostream &os, const Token &tkn )
        {
            for(const Char *ch = tkn.head;ch;ch=ch->next)
            {
                os << char(ch->code);
            }
            return os;
        }

        string Token::toString() const
        {
        }
        

    }
}

