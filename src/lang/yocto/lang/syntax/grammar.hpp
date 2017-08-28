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

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Grammar);
            };
        }
    }
}


#endif
