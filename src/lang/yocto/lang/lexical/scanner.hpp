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
            class   Translator; //!< forward declaration
            class   Plugin;     //!< forward declaration

            //! callback to be taken on a change of scanner
            typedef functor<void,TL1(const Token &)> Callback;


            //! simple lexical scanner, think 'flex'
            class Scanner : public counted_object
            {
            public:
                typedef intr_ptr<string,Scanner> Handle;

                const Stamp   stamp; //!< dynamic stamp
                const string &label; //!< *stamp

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

                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void jump(const string &label,
                          const string &expr,
                          OBJECT_POINTER host,
                          METHOD_POINTER meth)
                {
                    const Callback cb(host,meth);
                    jump(label,RegExp(expr,sharedDict),cb);
                }

                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void jump(const char    *label__,
                          const char    *expr__,
                          OBJECT_POINTER host,
                          METHOD_POINTER meth)
                {
                    const string label(label__);
                    const string expr(expr__);
                    jump(label,expr,host,meth);
                }



                //______________________________________________________________
                //
                //
                // call
                //
                //______________________________________________________________
                void call(const string   &scanr,
                          Pattern        *motif,
                          const Callback &onCall);

                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void call(const string &label,
                          const string &expr,
                          OBJECT_POINTER host,
                          METHOD_POINTER meth)
                {
                    const Callback cb(host,meth);
                    call(label,RegExp(expr,sharedDict),cb);
                }

                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void call(const char    *label__,
                          const char    *expr__,
                          OBJECT_POINTER host,
                          METHOD_POINTER meth)
                {
                    const string label(label__);
                    const string expr(expr__);
                    call(label,expr,host,meth);
                }

                void call(const string &pluginID);

                //______________________________________________________________
                //
                //
                // back
                //
                //______________________________________________________________
                void back(Pattern        *motif,
                          const Callback &onBack);

                
                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void back(const string   &expr,
                          OBJECT_POINTER host,
                          METHOD_POINTER meth)
                {
                    const Callback cb(host,meth);
                    back(RegExp(expr,sharedDict),cb);
                }

                template <
                typename OBJECT_POINTER,
                typename METHOD_POINTER>
                inline
                void back(const char    *expr__,
                          OBJECT_POINTER host,
                          METHOD_POINTER meth)
                {
                    const string   expr(expr__);
                    back(expr,host,meth);
                }


                //______________________________________________________________
                //
                //
                // probing function
                //
                //______________________________________________________________
                Unit *probe(Source &source, Result &kind);

                //! create a new empty unit with this->stamp
                Unit *newUnit() const;

                //! create a unit with this->stamp and steal content
                Unit *newUnit( Token &content ) const;

                const Rule *head() const throw();


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Scanner);
                Rules rules;
                void checkRuleName(const string &) const;
                
            public:
                Module *module;
                const size_t       max_label_length;
                const PatternDict *sharedDict;
                
            protected:
                friend class Translator;
                Translator        *translator;
            };
        }
    }
}

#endif
