#ifndef YOCTO_LANG_LEXICAL_PLUGIN_CSTRING_INCLUDED
#define YOCTO_LANG_LEXICAL_PLUGIN_CSTRING_INCLUDED 1

#include "yocto/lang/lexical/plugin.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            class cstring : public Plugin
            {
            public:
                virtual ~cstring() throw();
                explicit cstring(const char *id, Translator &trans);

                YOCTO_LANG_PLUGIN_DECL();

            private:
                Token data;
                YOCTO_DISABLE_COPY_AND_ASSIGN(cstring);
                void   quit(const Token &);
                Result grow(const Token &) throw();
            };

        }
        
    }
    
}
#endif
