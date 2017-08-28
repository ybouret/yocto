#ifndef YOCTO_LANG_SYNTAX_TERMINAL_INCLUDED
#define YOCTO_LANG_SYMTAX_TERMINAL_INCLUDED 1

#include "yocto/lang/syntax/rule.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            //! admit a lexeme with the same label
            class Terminal : public Rule
            {
            public:
                static const uint32_t UUID = YOCTO_FOURCC('T', 'E', 'R', 'M');
                explicit Terminal(const string &id);
                virtual ~Terminal() throw();
                
                YOCTO_LANG_SYNTAX_RULE_DECL();
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Terminal);
            };
        }
    }
}

#endif

