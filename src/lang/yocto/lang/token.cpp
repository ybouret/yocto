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
                os << ch->text();
            }
            return os;
        }

        string Token::toString() const
        {
            string ans(size,as_capacity);
            for(const Char *ch=head;ch;ch=ch->next)
            {
                ans.append(ch->code);
            }
            return ans;
        }
        
        void Token:: add( const Token &other )
        {
            for(const Char *ch = other.head;ch;ch=ch->next)
            {
                push_back( new Char(*ch) );
            }
        }

        int Token:: line()   const throw()
        {
            return (size>0) ? head->line : 0;
        }

        int Token:: column()   const throw()
        {
            return (size>0) ? head->column : 0;
        }

        const char *Token:: moduleName() const throw()
        {
            return (size>0) ? head->stamp->c_str() : "";
        }


    }
}

