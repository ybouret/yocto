#ifndef YOCTO_LANG_CHAR_INCLUDED
#define YOCTO_LANG_CHAR_INCLUDED 1

#include "yocto/lang/char-info.hpp"

namespace yocto
{

    namespace Lang
    {
        
        
        //! a dynamic char, based on a CharInfo
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

            const char *text() const throw();

        private:
            YOCTO_DISABLE_ASSIGN(Char);
            mutable char _txt[4];
        };

    }

}

#endif
