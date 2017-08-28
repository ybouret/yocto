#ifndef YOCTO_LANG_SYNTAX_OPTIONAL_INCLUDED
#define YOCTO_LANG_SYNTAX_OPTIONAL_INCLUDED 1

#include "yocto/lang/syntax/joker.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            class Optional : public Joker
            {
            public:
                static const uint32_t UUID = YOCTO_FOURCC('?', ' ', ' ', ' ');
                virtual ~Optional() throw();
                explicit Optional(const string &,Rule &);
                YOCTO_LANG_SYNTAX_RULE_DECL();
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Optional);
            };
            
        }
        
    }
}


#endif
