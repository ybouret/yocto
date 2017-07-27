#ifndef YOCTO_LANG_TOKEN_INCLUDED
#define YOCTO_LANG_TOKEN_INCLUDED 1

#include "yocto/lang/char.hpp"
#include "yocto/core/list.hpp"

namespace yocto
{
    namespace Lang
    {
        typedef core::list_of_cpp<Char> Chars;

        //! a token
        class Token : public object, public Chars
        {
        public:
            explicit Token() throw();
            virtual ~Token() throw();
            Token(const Token &other);

            friend std::ostream & operator<<( std::ostream&, const Token &);
            string toString() const;

        private:
            YOCTO_DISABLE_ASSIGN(Token);
        };

    }
}

#endif

