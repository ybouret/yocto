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
                explicit Comment(const char *id,
                                 const char *expr,
                                 Translator &trans);
                explicit Comment(const string &id,
                                 const string &expr,
                                 Translator   &trans);
                
                virtual ~Comment() throw();

                YOCTO_LANG_PLUGIN_DECL();
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Comment);
                const string data;
                void  onLeave(const Token &) throw();
                void init();
            };

            class InlineComment : public Plugin
            {
            public:
                explicit InlineComment(const char *id,
                                       const char *exprIni,
                                       const char *exprEnd,
                                       Translator &trans);
                explicit InlineComment(const string &id,
                                       const string &exprIni,
                                       const string &exprEnd,
                                       Translator   &trans);

                virtual ~InlineComment() throw();
                YOCTO_LANG_PLUGIN_DECL();

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(InlineComment);
                const string __ini;
                const string __end;
                void  onLeave(const Token &) throw();
                void init();
            };
        }
    }
}


#endif
