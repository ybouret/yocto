#ifndef YOCTO_LANG_GEN_GCOMPILER_INCLUDED
#define YOCTO_LANG_GEN_GCOMPILER_INCLUDED 1

#include "yocto/lang/gen/gparser.hpp"
#include "yocto/threading/singleton.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            class gCompiler : public singleton<gCompiler>
            {
            public:
                gParser getAST;
                Parser *encode(const Node *ast);
                bool    verbose;


            private:
                explicit gCompiler();
                virtual ~gCompiler() throw();
                YOCTO_DISABLE_COPY_AND_ASSIGN(gCompiler);
                static const char                 name[];
                static const threading::longevity life_time = -10;
                friend class singleton<gCompiler>;

                void prepareTopLevel(const Node *node);
            };

        }
    }

}

#endif
