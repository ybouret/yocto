#ifndef YOCTO_SYNTAX_RULE_INCLUDED
#define YOCTO_SYNTAX_RULE_INCLUDED 1



#include "yocto/lang/lexical/translator.hpp"
#include "yocto/lang/syntax/node.hpp"

#define YOCTO_LANG_SYNTAX_VERBOSE 0

#if (1==YOCTO_LANG_SYNTAX_VERBOSE)
#include <iostream>
#define YOCTO_LANG_SYNTAX(CODE) do { for(int __ii=0;__ii<depth;++__ii) { std::cerr << "  "; } CODE; } while(false)
#else
#define YOCTO_LANG_SYNTAX(CODE)
#endif

namespace yocto
{
    namespace Lang
    {
        enum SyntaxRuleFlag
        {
            IsNormal = 0x00, //!< different possible tokens
            IsUnique = 0x01, //!< always the same token, clear content (aka raw string...)
            IsHollow = 0x02, //!< no semantic meaning (aka separators...)
            NoSingle = 0x04  //!< simplify is only one child
        };

        namespace Syntax
        {

            typedef Lexical::Translator Lexer;

#define YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS Node * &tree, Lexer &lexer, Source &source, int &depth

#define YOCTO_LANG_SYNTAX_RULE_DECL()                                  \
virtual bool          admit(YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS) const;  \
virtual void          __viz(ios::ostream &fp) const;                   \
virtual void          __lnk(ios::ostream &fp) const;                   \
virtual const char *  __shp() const throw();                           \
virtual bool          admitsEmpty() const throw()

#define YOCTO_LANG_SYNTAX_RULE_SELF() self_ = this

            //! Rule interface
            /**
             - flags are for terminal
             - if the label contains the FusionMark, then the content will be merged to parent
             */
            class Rule : public object
            {
            public:
                typedef core::list_of_cpp<Rule> List;
                static const  char              FusionMark = '#';

                Rule          *next;
                Rule          *prev;
                const uint32_t uuid;
                const string   label;
                const uint32_t flags;
                void          *self_;

                Rule &let(const uint32_t flag) throw();

                virtual ~Rule() throw();

                //______________________________________________________________
                //
                // virtual interface
                //______________________________________________________________

                virtual bool        admit(YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS) const = 0;
                virtual void        __viz(ios::ostream &) const                    = 0;
                virtual void        __lnk(ios::ostream &) const                    = 0;
                virtual const char *__shp()               const throw()            = 0;
                virtual bool          admitsEmpty() const throw()                  = 0;
                
                //______________________________________________________________
                //
                // non-virtual interface
                //______________________________________________________________
                const string &key() const throw();
                static void Restore( Lexer &lexer, Node *node ) throw();
                void __mark( ios::ostream &fp ) const;

                inline const char   *id() const throw() { return label.c_str(); }

            protected:
                explicit Rule(const string  &id,
                              const uint32_t u);

                explicit Rule(const char    *id,
                              const uint32_t u);
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Rule);

            };
        }
    }
}

#endif

