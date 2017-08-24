#ifndef YOCTO_LANG_LEXICAL_SCANNER_INCLUDED
#define YOCTO_LANG_LEXICAL_SCANNER_INCLUDED 1

#include "yocto/lang/lexical/rule.hpp"
#include "yocto/lang/lexical/unit.hpp"
#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/ptr/intr.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            class   Translator;
            typedef functor<void,TL1(const Token &)> Callback;

            class Scanner : public counted_object
            {
            public:
                typedef intr_ptr<string,Scanner> Handle;

                const Stamp   stamp;
                const string &label;

                explicit Scanner(const string &id);
                explicit Scanner(const char   *id);

                virtual ~Scanner() throw();

                const string & key() const throw();

                //______________________________________________________________
                //
                //
                // make
                //
                //______________________________________________________________

                //______________________________________________________________
                //
                // take action upon motif with label
                //______________________________________________________________
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


                //______________________________________________________________
                //
                // take action upon expression with label
                //______________________________________________________________
                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void make(const string  &label,
                          const string  &expr,
                          OBJECT_POINTER host,
                          METHOD_POINTER meth)
                {
                    make(label,RegExp(expr,sharedDict),host,meth);
                }

                //______________________________________________________________
                //
                // take action upon expression with label, wrappers
                //______________________________________________________________
                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void make(const char    *label__,
                          const char    *expr__,
                          OBJECT_POINTER host,
                          METHOD_POINTER meth)
                {
                    const string label(label__);
                    const string expr(expr__);
                    make(label,expr,host,meth);
                }

                //______________________________________________________________
                //
                //
                // jump
                //
                //______________________________________________________________
                void jump(const string   &scanr,
                          Pattern        *motif,
                          const Callback &onJump);

                //______________________________________________________________
                //
                //
                // call
                //
                //______________________________________________________________
                void call(const string   &scanr,
                          Pattern        *motif,
                          const Callback &onCall);
                

                //______________________________________________________________
                //
                //
                // back
                //
                //______________________________________________________________
                void back(Pattern        *motif,
                          const Callback &onBack);
                //______________________________________________________________
                //
                //
                // probing function
                //
                //______________________________________________________________
                Unit *probe(Source &source, Result &kind);

                //! create a new empty unit
                Unit *newUnit() const;


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Scanner);
                Rules rules;
                void checkRuleName(const string &) const;
                
            public:
                Module *module;
                const size_t       max_label_length;
                const PatternDict *sharedDict;

            private:
                friend class Translator;
                Translator        *translator;
            };
        }
    }
}

#endif
