#ifndef YOCTO_LANG_LEXICAL_SCANNER_INCLUDED
#define YOCTO_LANG_LEXICAL_SCANNER_INCLUDED 1

#include "yocto/lang/lexical/rule.hpp"
#include "yocto/lang/lexical/unit.hpp"
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
                explicit Scanner(const char   *id);

                virtual ~Scanner() throw();

                const string & key() const throw();

                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void make(const string  &label,
                          const Pattern *motif,
                          OBJECT_POINTER host,
                          METHOD_POINTER meth)
                {
                    const Pattern::Handle handle(motif);
                    checkRuleName(label);
                    const Action action(host,meth);
                    rules.push_back( new Rule(label,handle,action) );
                }

                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void make(const char    *label__,
                          const Pattern *motif,
                          OBJECT_POINTER host,
                          METHOD_POINTER meth)
                {
                    const Pattern::Handle handle(motif);
                    const string          label(label__);
                    checkRuleName(label);
                    const Action action(host,meth);
                    rules.push_back( new Rule(label,handle,action) );
                }
                
                Unit *probe(Source &source, bool &isRegular);




            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Scanner);
                Rules rules;
                void checkRuleName(const string &label) const;

            protected:
                Module *module;
                
            public:
                const size_t max_label_length;
                
            };
        }
    }
}

#endif
