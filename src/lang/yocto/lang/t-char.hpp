#ifndef YOCTO_LANG_T_CHAR_INCLUDED
#define YOCTO_LANG_T_CHAR_INCLUDED 1

#include "yocto/lang/module.hpp"

namespace yocto
{

    namespace Lang
    {
        

        class  Char :
        public CharInfo,
        public object
        {
        public:
            t_char *prev;
            t_char *next;
            uint8_t code;
            
            
            

        private:
            YOCTO_DISABLE_ASSIGN(t_char);
        };

    }

}

#endif
