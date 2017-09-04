#ifndef YOCTO_LANG_LEXICAL_STRING64_INCLUDED
#define YOCTO_LANG_LEXICAL_STRING64_INCLUDED 1

#include "yocto/lang/lexical/plugin.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            class String64 : public Plugin
            {
            public:
                virtual ~String64() throw();
                explicit String64(const char   *id, Translator &trans);
                explicit String64(const string &id, Translator &trans);

                YOCTO_LANG_PLUGIN_DECL();

            private:
                Token b64;
                void   init();
                void   quit(const Token&);
                Result grow(const Token &);
                Result excp(const Token &);
                
                YOCTO_DISABLE_COPY_AND_ASSIGN(String64);


            };

        }
    }
}

#endif
