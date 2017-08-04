#ifndef YOCTO_LANG_LEXICAL_SCANNER_INCLUDED
#define YOCTO_LANG_LEXICAL_SCANNER_INCLUDED 1

#include "yocto/lang/lexical/rule.hpp"
#include "yocto/lang/source.hpp"
#include "yocto/ptr/intr.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            class Scanner : public counted_object
            {
            public:
                typedef intr_ptr<string,Scanner> Handle;

                const string name;

                explicit Scanner(const string &id);
                virtual ~Scanner() throw();

                const string & key() const throw();

                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void define(const string  &label,
                            const Pattern *motif,
                            OBJECT_POINTER host,
                            METHOD_POINTER meth)
                {
                    auto_ptr<const Pattern> guard(motif);
                    checkRuleName(label);
                    guard.forget();
                    const Action action(host,meth);
                    rules.push_back( Rule::Create(label,motif,action) );
                }

                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void define(const char    *label__,
                            const Pattern *motif,
                            OBJECT_POINTER host,
                            METHOD_POINTER meth)
                {
                    auto_ptr<const Pattern> guard(motif);
                    const string            label(label__);
                    checkRuleName(label);
                    guard.forget();
                    const Action action(host,meth);
                    rules.push_back( Rule::Create(label,motif,action) );
                }



            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Scanner);
                Rules rules;
                void checkRuleName(const string &label) const;
            };
        }
    }
}

#endif
