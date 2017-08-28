#ifndef YOCTO_LANG_SYNTAX_GRAMMAR_INCLUDED
#define YOCTO_LANG_SYNTAX_GRAMMAR_INCLUDED 1

#include "yocto/lang/syntax/rule.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            class Grammar
            {
            public:
                virtual ~Grammar() throw();
                explicit Grammar(const string &id);

                const string name;

                Rule &append( Rule *rule );
                void  setTopLevel( Rule &rule );
                Rule *getTopLevel() const throw();

                Node *accept(Lexer &lexer, Source &source);

            private:
                Rule::List rules;

                YOCTO_DISABLE_COPY_AND_ASSIGN(Grammar);
            };
        }
    }
}


#endif
