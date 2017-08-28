#ifndef YOCTO_LANG_SYNTAX_PARSER_INCLUDED
#define YOCTO_LANG_SYNTAX_PARSER_INCLUDED 1

#include "yocto/lang/syntax/grammar.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            class Parser : public Grammar, public Lexer
            {
            public:
                explicit Parser(const string &id);
                virtual ~Parser() throw();
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Parser);
            };

            
        }
    }
}

#endif

