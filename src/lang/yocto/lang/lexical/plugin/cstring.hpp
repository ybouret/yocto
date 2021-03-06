#ifndef YOCTO_LANG_LEXICAL_PLUGIN_CSTRING_INCLUDED
#define YOCTO_LANG_LEXICAL_PLUGIN_CSTRING_INCLUDED 1

#include "yocto/lang/lexical/plugin.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {



            //! a plugin to return a C-string as 'this->label'
            class _String : public Plugin
            {
            public:
                virtual ~_String() throw();
                YOCTO_LANG_PLUGIN_DECL();

            protected:
                explicit _String(const char   *id, Translator &trans,const char *ch);
                explicit _String(const string &id, Translator &trans,const char *ch);


            private:
                Token        data;
                const string __ch;
                YOCTO_DISABLE_COPY_AND_ASSIGN(_String);
                void   init();
                void   quit(const Token &);
                Result grow(const Token &);
                Result escCtrl(const Token &);
                Result escCopy(const Token &);
                Result escHexa(const Token &);
                Result escFailure(const Token &);
            };

            class cstring : public _String
            {
            public:
                explicit cstring(const char   *id, Translator &trans);
                explicit cstring(const string &id, Translator &trans);
                virtual ~cstring() throw();

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(cstring);
                static const char Expr[];
            };


            class rstring : public _String
            {
            public:
                explicit rstring(const char   *id, Translator &trans);
                explicit rstring(const string &id, Translator &trans);
                virtual ~rstring() throw();

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(rstring);
                static const char Expr[];
            };


        }
        
    }
    
}
#endif
