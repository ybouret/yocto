#ifndef YOCTO_LANG_TOKEN_INCLUDED
#define YOCTO_LANG_TOKEN_INCLUDED 1

#include "yocto/lang/char.hpp"
#include "yocto/core/list.hpp"

namespace yocto
{
    namespace Lang
    {
        typedef core::list_of_cpp<Char> Chars;

        //! a token od dynamic chars
        class Token : public object, public Chars
        {
        public:
            explicit Token() throw();
            virtual ~Token() throw();
            Token(const Token &other);

            friend std::ostream & operator<<( std::ostream&, const Token &);
            string toString(const size_t nskip=0) const;

            void add(const Token &);

            int         line()   const throw(); //!< line of the first char
            int         column() const throw(); //!< column of the first char
            const char *moduleName() const throw();

        private:
            YOCTO_DISABLE_ASSIGN(Token);
        };

    }
}

#endif

