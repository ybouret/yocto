#ifndef YOCTO_LANG_SYNTAX_PARSER_INCLUDED
#define YOCTO_LANG_SYNTAX_PARSER_INCLUDED 1

#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/sequence/list.hpp"

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
                const string tag;
                RPN_Set      rdb;
                
                //______________________________________________________________
                //
                // terminals....
                //______________________________________________________________

                //! emit the terminal and make a rule out of it
                Terminal & terminal(const string &label,
                                    const string &expr);

                //! wrapper
                inline Terminal & terminal(const char   *label,
                                           const char   *expr)
                {
                    const string __label(label);
                    const string __expr(expr);
                    return terminal(__label,__expr);
                }

                //! expression as a label
                inline Terminal & terminal(const string &expr) { return terminal(expr,expr); }

                //! wrapper
                inline Terminal & terminal(const char   *expr) { const string __expr(expr); return terminal(__expr); }

                //! single chars
                Terminal & terminal(const string &label,
                                    const uint8_t code);

                //! wrapper
                inline Terminal & terminal(const char *label, const uint8_t code)
                {
                    const string __label(label);
                    return terminal(__label,code);
                }

                //! is hollow by default !
                inline Terminal & terminal(const uint8_t code)
                {
                    const char   C(code);
                    const string __label(C);
                    Terminal &t = terminal(__label,code);
                    (void) t.let(IsHollow);
                    return t;
                }

                //! is unique by default
                inline Terminal & modifier(const uint8_t code)
                {
                    const char   C(code);
                    const string __label(C);
                    Terminal &t = terminal(__label,code);
                    (void) t.let(IsUnique);
                    return t;
                }

                //______________________________________________________________
                //
                // special: plugins
                //______________________________________________________________
                template <typename PLUGIN>
                Terminal &term(const string &label)
                {
                    hook<PLUGIN>(label);
                    return plugin(label);
                }


                //! check validity
                void check() const;

                //! get the AST
                Node * parse(Source &source);

                //! generation
                static Parser *Generate(const string &filename,
                                        const bool    verbose=false);

                //! generation
                static Parser *Generate(const void    *buffer,
                                        const size_t   buflen,
                                        const bool     verbose=false);

                static void Encode(const string &filename,
                                   ios::ostream &output);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Parser);
                Terminal & plugin(const string &label);

            protected:
                list<Stamp> stamps;
            };
            
            
        }
    }
}

#endif

