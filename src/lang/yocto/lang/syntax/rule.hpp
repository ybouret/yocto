#ifndef YOCTO_SYNTAX_RULE_INCLUDED
#define YOCTO_SYNTAX_RULE_INCLUDED 1

#include "yocto/lang/lexical/translator.hpp"
#include "yocto/lang/syntax/node.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            typedef Lexical::Translator Lexer;

#define YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS Lexer &lexer
#define YOCTO_LANG_SYNTAX_RULE_DECL() \
virtual bool admit(YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS) const

            class Rule : public object
            {
            public:
                Rule          *next;
                Rule          *prev;
                const uint32_t uuid;
                const string   label;

                virtual ~Rule() throw();

                //______________________________________________________________
                //
                // virtual interface
                //______________________________________________________________

                virtual bool admit(YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS) const = 0;


                //______________________________________________________________
                //
                // non-virtual interface
                //______________________________________________________________
                const string &key() const throw();

                
            protected:
                explicit Rule(const string  &id,
                              const uint32_t u);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Rule);
            };
        }
    }
}

#endif

