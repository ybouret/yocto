#ifndef YOCTO_LANG_SYNTAX_GRAMMAR_INCLUDED
#define YOCTO_LANG_SYNTAX_GRAMMAR_INCLUDED 1

#include "yocto/lang/syntax/rules.hpp"

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

                void  append( Rule *rule );
                void  setTopLevel( Rule &rule );
                Rule *getTopLevel() const throw();

                Node *accept(Lexer &lexer, Source &source);

                template <typename RULE>
                RULE & add( RULE *rule )
                {
                    append(rule);
                    return *rule;
                }

                void graphviz(const string &filename) const;

                Rule & Choice( Rule &a, Rule &b );
                Rule & Choice( Rule &a, Rule &b, Rule &c);
                Rule & Option( Rule &a );
                Rule & OneOrMore( Rule &a );
                Rule & ZeroOrMore( Rule &a);
                
                Alternate &alt(); //!< automatic labeling
                Alternate &alt(const string &label);
                Alternate &alt(const char   *label);
                Aggregate &agg(const string &label);
                Aggregate &agg(const char   *label);
                

                void check_connectivity() const;

            private:
                Rule::List rules;
                int        altCount;
                int        optCount;
                int        zomCount;
                int        oomCount;
                int        aggCount;
                string     tmpLabel;
                YOCTO_DISABLE_COPY_AND_ASSIGN(Grammar);

                const string & newAltLabel();
                const string & newOptLabel();
                const string & newZomLabel();
                const string & newOomLabel();
                const string & newAggLabel();

            };
        }
    }
}


#endif
