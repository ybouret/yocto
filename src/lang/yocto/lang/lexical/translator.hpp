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
                const string name;


                //______________________________________________________________
                //
                // intialization
                //______________________________________________________________
                explicit Translator(const string &transID);
                explicit Translator(const char *transID);
                virtual ~Translator() throw();

                //______________________________________________________________
                //
                // sub-scanners
                //______________________________________________________________
                Scanner &declare(const string &scanrID);
                Scanner &declare(const char   *scanrID);

                void __jump(const string &id);
                void __call(const string &id);
                void __back();


                //______________________________________________________________
                //
                // I/O
                //______________________________________________________________

                //! get next unit from source
                Unit *get( Source &source );

                //! lookahead
                const Unit *peek( Source &source );

                //! dispatch to current module
                void  newLine() throw();

                //! unget a unit
                void  unget( Unit *u ) throw();

                //! cleanup
                void  reset() throw();

                //______________________________________________________________
                //
                // methods for simple use with scanners.make
                //______________________________________________________________
                Result forward(const Token &) throw();
                Result discard(const Token &) throw();
                Result newline(const Token &) throw();

                //______________________________________________________________
                //
                // Plugin API
                //______________________________________________________________
                const string &plug( Plugin *p );

                template <typename PLUGIN> inline
                const string &hook(const string &id)
                {
                    Plugin *p = new PLUGIN(id,*this);
                    return plug( p );
                }

                template <typename PLUGIN> inline
                const string & hook(const char *id)
                {
                    Plugin *p = new PLUGIN(id,*this);
                    return plug( p );
                }
                
                
                template <typename PLUGIN> inline
                const string & hook( const string &id,const string & args )
                {
                    Plugin *p = new PLUGIN(id,args,*this);
                    return plug(p);
                }

                template <typename PLUGIN> inline
                const string & hook( const char *id,const char *args )
                {
                    Plugin *p = new PLUGIN(id,args,*this);
                    return plug(p);
                }

                template <typename PLUGIN> inline
                const string & hook( const char *id,const char *arg1,const char *arg2)
                {
                    Plugin *p = new PLUGIN(id,arg1,arg2,*this);
                    return plug(p);
                }

                template <typename PLUGIN> inline
                const string & hook(const string &id,const string &arg1,const string &arg2)
                {
                    Plugin *p = new PLUGIN(id,arg1,arg2,*this);
                    return plug(p);
                }


                typedef set<string,Scanner::Handle> ScannerDB;
                
            private:
                Scanner::Handle  _root;
                Units            units;
            public:
                Scanner         &root;

            private:
                Units            cache;

                friend class Scanner;
                friend class Plugin;
                
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
