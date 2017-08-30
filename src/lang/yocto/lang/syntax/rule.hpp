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

#define YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS Node * &tree, Lexer &lexer, Source &source

#define YOCTO_LANG_SYNTAX_RULE_DECL()                                  \
virtual bool          admit(YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS) const;  \
virtual void          __viz(ios::ostream &fp) const;                   \
virtual const char *  __shp() const throw();                           \
virtual bool          admitsEmpty() const throw()

#define YOCTO_LANG_SYNTAX_RULE_SELF() self_ = this

            class Rule : public object
            {
            public:
                static const uint32_t IsNormal = 0x00; //!< different possible tokens
                static const uint32_t IsUnique = 0x01; //!< always the same token
                static const uint32_t IsHollow = 0x02; //!< no semantic meaning

                typedef core::list_of_cpp<Rule> List;
                Rule          *next;
                Rule          *prev;
                const uint32_t uuid;
                const string   label;
                const uint32_t flags;
                void          *self_;

                Rule &impose(const uint32_t flag) throw();

                virtual ~Rule() throw();

                //______________________________________________________________
                //
                // virtual interface
                //______________________________________________________________

                virtual bool        admit(YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS) const = 0;
                virtual void        __viz(ios::ostream &) const                    = 0;
                virtual const char *__shp()               const throw()            = 0;
                virtual bool          admitsEmpty() const throw()                  = 0;
                
                //______________________________________________________________
                //
                // non-virtual interface
                //______________________________________________________________
                const string &key() const throw();
                static void Restore( Lexer &lexer, Node *node ) throw();
                void __mark( ios::ostream &fp ) const;

                
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

