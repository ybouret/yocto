#ifndef YOCTO_LANG_SYNTAX_AGGREGATE_INCLUDED
#define YOCTO_LANG_SYNTAX_AGGREGATE_INCLUDED 1

#include "yocto/lang/syntax/compound.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            class Aggregate : public Compound
            {
            public:
                static const uint32_t UUID = YOCTO_FOURCC(' ','&','*',' ');
                virtual ~Aggregate() throw();
                explicit Aggregate(const string &);

                YOCTO_LANG_SYNTAX_RULE_DECL();

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Aggregate);
            };
            
        }
    }
}

#endif
