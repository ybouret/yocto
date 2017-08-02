#ifndef YOCTO_LANG_LEXEME_INCLUDED
#define YOCTO_LANG_LEXEME_INCLUDED 1

#include "yocto/lang/source.hpp"

namespace yocto
{
    namespace Lang
    {

        class Lexeme : public Token
        {
        public:
            const string &label;
            Lexeme       *next;
            Lexeme       *prev;

            explicit Lexeme(const string &usrLabel) throw();
            virtual ~Lexeme() throw();

        private:
            YOCTO_DISABLE_ASSIGN(Lexeme);
        };

    }
}

#endif
