#ifndef YOCTO_LANG_SYNTAX_ALTERNATE_INCLUDED
#define YOCTO_LANG_SYNTAX_ALTERNATE_INCLUDED 1

#include "yocto/lang/syntax/compound.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            //! create an internal alternate rule
            class Alternate : public Compound
            {
            public:
                static const uint32_t UUID = YOCTO_FOURCC(' ','|','|',' ');
                virtual ~Alternate() throw();
                explicit Alternate(const string &);
                explicit Alternate(const char   *);

                YOCTO_LANG_SYNTAX_RULE_DECL();
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Alternate);
            };
            
            

        }
    }
}

#endif
