#ifndef YOCTO_LANG_LEXICAL_TRANSLATOR_INCLUDED
#define YOCTO_LANG_LEXICAL_TRANSLATOR_INCLUDED 1

#include "yocto/lang/lexical/plugin.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/core/pool.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            class Translator 
            {
            public:

                explicit Translator(const string &transID);
                
                explicit Translator(const char *transID);

                virtual ~Translator() throw();


                Scanner &declare(const string &scanrID);
                Scanner &declare(const char   *scanrID);

                void __jump(const string &id);
                void __call(const string &id);
                void __back();


                const string name;

                //! get next unit from source
                Unit *get( Source &source );

                //! dispatch to current module
                void  newLine() throw();

                //! unget a unit
                void  unget( Unit *u ) throw();

                //! cleanup
                void  reset() throw();

                Result forward(const Token &) throw();
                Result discard(const Token &) throw();
                Result newline(const Token &) throw();

                void plug( Plugin *p );

                template <typename PLUGIN> inline
                void hook()
                {
                    Plugin *p = new PLUGIN();
                    plug( p );
                }

                template <typename PLUGIN,typename ARGS> inline
                Plugin & hook( typename type_traits<ARGS>::parameter_type args )
                {
                    Plugin *p = new PLUGIN(args);
                    plug(p);
                }


            private:
                Scanner::Handle  _root;
                Units            units;
            public:
                Scanner         &root;

            private:
                Units            cache;

                friend class Scanner;
                class sNode : public object
                {
                public:
                    sNode   *next;
                    Scanner *addr;
                    explicit sNode(Scanner *) throw();
                    virtual ~sNode() throw();

                private:
                    YOCTO_DISABLE_COPY_AND_ASSIGN(sNode);
                };

                typedef core::pool_of_cpp<sNode>    History;
                typedef set<string,Scanner::Handle> ScannerDB;
                typedef set<string,Plugin::Handle>  PluginDB;
                
                Scanner        *current;  //!< current scanner
                ScannerDB       scanners; //!< the scanners
                History         history;
                PluginDB        plugins;

                void            onInit();
                void            linkTo( Scanner & ) throw();

            public:
                PatternDict     dict;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Translator);
            };

#define YOCTO_LANG_LEXICAL(METHOD) this, & yocto::Lang::Lexical::Translator::METHOD
            
        }
    }
}

#endif
