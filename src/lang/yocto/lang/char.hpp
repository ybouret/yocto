#ifndef YOCTO_LANG_CHAR_INCLUDED
#define YOCTO_LANG_CHAR_INCLUDED 1

#include "yocto/lang/module.hpp"

namespace yocto
{

    namespace Lang
    {
        
        
        //! a dynamic char
        class  Char :
        public CharInfo,
        public object
        {
        public:
            Char   *prev;
            Char   *next;
            uint8_t code;
            virtual ~Char() throw();
            explicit Char(const CharInfo &info,
                          const uint8_t   data) throw();
            
            Char(const Char &other) throw();

        private:
            YOCTO_DISABLE_ASSIGN(Char);
        };

    }

}

#endif
