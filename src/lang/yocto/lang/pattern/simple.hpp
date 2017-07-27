#ifndef YOCTO_LANG_PATTERN_SIMPLE_INCLUDED
#define YOCTO_LANG_PATTERN_SIMPLE_INCLUDED 1

#include "yocto/lang/pattern.hpp"

namespace yocto
{

    namespace Lang
    {

        class Any1 : public Pattern
        {
        public:
            YOCTO_LANG_PATTERN_DECL(Any1,'A', 'N', 'Y', '1');

            virtual ~Any1() throw();
            explicit Any1() throw();


        private:
            YOCTO_DISABLE_ASSIGN(Any1);
            Any1(const Any1 &) throw();
        };

    }
}

#endif


