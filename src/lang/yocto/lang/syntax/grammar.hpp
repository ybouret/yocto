#ifndef YOCTO_LANG_SYNTAX_GRAMMAR_INCLUDED
#define YOCTO_LANG_SYNTAX_GRAMMAR_INCLUDED 1

#include "yocto/lang/syntax/rules.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            //! a grammar is a set of rules
            class Grammar
            {
            public:
                virtual ~Grammar() throw();
                explicit Grammar(const string &id);

                const string name;

                void  append( Rule *rule );
                void  setTopLevel( Rule &rule );
                Rule *getTopLevel() const throw();

                Node *accept(Lexer &lexer, Source &source);

                template <typename RULE>
                inline RULE & add( RULE *rule )
                {
                    append(rule);
                    return *rule;
                }

                template <typename RULE> static inline
                RULE & Decl( RULE &r, const uint32_t flag)
                {
                    r.let(flag);
                    return r;
                }
                
                void graphviz(const string &filename) const;

                Rule & choice( Rule &a, Rule &b );
                Rule & choice( Rule &a, Rule &b, Rule &c);
                Rule & optional( Rule &a );
                Rule & oneOrMore( Rule &a );
                Rule & zeroOrMore( Rule &a);
                
                Alternate &alt(); //!< automatic labeling
                Alternate &alt(const string &label);
                Alternate &alt(const char   *label);
                Aggregate &agg(const string &label);
                Aggregate &agg(const char   *label);
                

                void check_connectivity() const;


                const string & newAltLabel();
                const string & newOptLabel();
                const string & newZomLabel();
                const string & newOomLabel();
                const string & newAggLabel();

            private:
                Rule::List rules;
                int        altCount;
                int        optCount;
                int        zomCount;
                int        oomCount;
                int        aggCount;
                string     tmpLabel;
                YOCTO_DISABLE_COPY_AND_ASSIGN(Grammar);



            };
        }
    }
}


#endif
