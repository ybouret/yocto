#ifndef YOCTO_LANG_GEN_GPARSER_INCLUDED
#define YOCTO_LANG_GEN_GPARSER_INCLUDED 1

#include "yocto/lang/syntax/parser.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            class gParser : public Parser
            {
            public:
                virtual ~gParser() throw();
                explicit gParser();
                
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(gParser);

            };
        }
    }
}

#endif
