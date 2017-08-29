#ifndef YOCTO_LANG_LEXICAL_PLUGIN_INCLUDED
#define YOCTO_LANG_LEXICAL_PLUGIN_INCLUDED 1

#include "yocto/lang/lexical/scanner.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            class Plugin : public Scanner
            {
            public:
                typedef intr_ptr<string,Plugin> Handle;
                
                virtual ~Plugin() throw();

            protected:
                explicit Plugin(const string &id);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Plugin);
            };
        }
    }
}

#endif
