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

                //! emit the terminal and make a rule out of it
                Rule & terminal(const string &label,
                                const string &expr);

                //! wrapper
                inline Rule & terminal(const char   *label,
                                       const char   *expr)
                {
                    const string __label(label);
                    const string __expr(expr);
                    return terminal(__label,__expr);
                }

                //! expression as a label
                inline Rule & terminal(const string &expr) { return terminal(expr,expr); }

                //! wrapper
                inline Rule & terminal(const char   *expr) { const string __expr(expr); return terminal(__expr); }

                //! single chars
                Rule & terminal(const string &label,
                                const uint8_t code);

                //! wrapper
                inline Rule & terminal(const char *label, const uint8_t code)
                {
                    const string __label(label);
                    return terminal(__label,code);
                }

                Node * parse(Source &source);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Parser);
            };

            
        }
    }
}

#endif
