#ifndef YOCTO_LANG_LEXICAL_UNIT_INCLUDED
#define YOCTO_LANG_LEXICAL_UNIT_INCLUDED 1

#include "yocto/lang/source.hpp"

namespace yocto
{
    namespace Lang
    {

        class Unit : public Token
        {
        public:
            virtual ~Unit() throw();
            
            const Stamp label;
            
        private:
            YOCTO_DISABLE_ASSIGN(Unit);
        };

    }
}

#endif
