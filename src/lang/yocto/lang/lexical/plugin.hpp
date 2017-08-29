#ifndef YOCTO_LANG_LEXICAL_PLUGIN_INCLUDED
#define YOCTO_LANG_LEXICAL_PLUGIN_INCLUDED 1

#include "yocto/lang/lexical/scanner.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

#define YOCTO_LANG_PLUGIN_DECL() \
virtual const char * trigger() const throw(); \
virtual void         onEnter(const Token & )

            class Plugin : public Scanner
            {
            public:
                typedef intr_ptr<string,Plugin> Handle;
                
                virtual ~Plugin() throw();

                virtual const char * trigger() const throw()   = 0;
                virtual void         onEnter(const Token&)     = 0;

            protected:
                explicit Plugin(const string &id);
                explicit Plugin(const char   *id);
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Plugin);
                virtual void         startUp(const Token &)    = 0;

            };
        }
    }
}

#endif
