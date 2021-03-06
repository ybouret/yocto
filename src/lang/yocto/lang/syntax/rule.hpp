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
            IsNormal     = 0x0000, //!< different possible tokens
            IsUnique     = 0x0001, //!< always the same token, clear content (aka raw string...)
            IsHollow     = 0x0002, //!< no semantic meaning (aka separators...)
            MergesSingle = 0x0100, //!< merges if only one child
            MergesAlways = 0x0200  //!< merges always, a temporary sub expression
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

#define YOCTO_LANG_SYNTAX_RULE_SELF(CLASS) self_ = static_cast<CLASS *>(this)

            //! Rule interface
            /**
             - flags are for terminal or aggregate
             */
            class Rule : public object
            {
            public:
                typedef core::list_of_cpp<Rule> List;
                static  const char InternalMark = '#';

                Rule          *next;     //!< for rules
                Rule          *prev;     //!< for rules
                const uint32_t uuid;     //!< to retrieve type
                const string   label;    //!< to match a terminal or standalone name
                const uint32_t flags;    //!< semantic modifiers
                void          *self_;    //!< on the typed address
                Tag            tag;      //!< for Dynamo
                
                void let(const uint32_t flag) throw();
                
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
                const char         *__sty() const throw();

                //______________________________________________________________
                //
                // non-virtual interface
                //______________________________________________________________
                const string &key() const throw();
                static void Restore( Lexer &lexer, Node *node ) throw();
                void __mark( ios::ostream &fp ) const;

                const char   *id() const throw();

                template <typename RULE>
                static inline RULE & TagWith(const Tag &t, RULE &r) throw() { r.tag = t; return r; }

            protected:
                explicit Rule(const string  &i,
                              const uint32_t u);

                explicit Rule(const char    *i,
                              const uint32_t u);
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Rule);

            };
        }
    }
}

#endif

