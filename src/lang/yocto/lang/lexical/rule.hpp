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
            /**

             */
            typedef functor<bool,const Token &> Action;

            class Rule : public object
            {
            public:
                Rule          *next;
                Rule          *prev;
                const Pattern *motif;
                Action         action;
                
                virtual ~Rule() throw();

                static Rule * Create(const Pattern *p, const Action &a);

#if 0
                template <typename OBJECT_POINTER, typename METHOD_POINTER>
                static inline
                Rule *Create(const Pattern *p, OBJECT_POINTER host, METHOD_POINTER method)
                {
                    auto_ptr<const Pattern> guard(p);
                    const Action            a(host,method);
                    Rule *r = new Rule(p,a);
                    guard.forget();
                    return r;
                }
#endif

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Rule);
                Rule(const Pattern *,const Action &);
            };

            typedef core::list_of_cpp<Rule> Rules;
        }
    }
}

#endif
