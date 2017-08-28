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

            enum Result
            {
                Forward, //!< will produce an unit
                Discard, //!< will discard the unit
                Control  //!< a control unit for translator
            };

            //! define an action
            typedef functor<Result,TL1(const Token &)> Action;

            class Rule : public object
            {
            public:
                Rule                 *next;
                Rule                 *prev;
                const Stamp           stamp;
                const string         &label;
                const Pattern::Handle motif;
                Action                action;

                virtual ~Rule() throw();

                Rule(const string          &,
                     const Pattern::Handle &,
                     const Action          &);


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Rule);

            };

            typedef core::list_of_cpp<Rule> Rules;
        }
    }
}

#endif
