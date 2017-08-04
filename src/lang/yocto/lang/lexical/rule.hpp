#ifndef YOCTO_LANG_LEXICAL_RULE_INCLUDED
#define YOCTO_LANG_LEXICAL_RULE_INCLUDED 1

#include "yocto/lang/token.hpp"
#include "yocto/lang/pattern.hpp"
#include "yocto/functor.hpp"
#include "yocto/ptr/auto.hpp"

namespace yocto
{
    namespace Lang
    {

        namespace Lexical
        {
            //! define an action
            typedef functor<bool,const Token &> Action;

            class Rule : public object
            {
            public:
                Rule          *next;
                Rule          *prev;
                const string   label;
                const Pattern *motif;
                Action         action;

                virtual ~Rule() throw();

                static Rule * Create(const string  &l,
                                     const Pattern *p,
                                     const Action  &a);


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Rule);
                Rule(const string &,const Pattern *,const Action &);
            };

            typedef core::list_of_cpp<Rule> Rules;
        }
    }
}

#endif
