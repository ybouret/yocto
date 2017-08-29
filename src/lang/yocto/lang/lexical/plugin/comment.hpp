#ifndef YOCTO_LANG_LEXICAL_PLUGIN_COMMENT_INCLUDED
#define YOCTO_LANG_LEXICAL_PLUGIN_COMMENT_INCLUDED 1

#include "yocto/lang/lexical/plugin.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            class Comment : public Plugin
            {
            public:
                explicit Comment(const char *id, const char *expr);
                virtual ~Comment() throw();

                YOCTO_LANG_PLUGIN_DECL();
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Comment);
                const string data;
            };

        }
    }
}


#endif
