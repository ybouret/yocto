#ifndef YOCTO_LANG_CHAR_INCLUDED
#define YOCTO_LANG_CHAR_INCLUDED 1

#include "yocto/lang/char-info.hpp"

namespace yocto
{

    namespace Lang
    {
        
        
        //! a dynamic char, based on a CharInfo
        class  Char : public object, public CharInfo
        {
        public:
            Char   *prev; //!< for token
            Char   *next; //!< for token
            uint8_t code; //!< any byte

            virtual ~Char() throw(); //!< default dtor

            //! build from info
            explicit Char(const CharInfo &info,
                          const uint8_t   data) throw();

            //! copy constructor
            Char(const Char &other) throw();

            //! make a representable state
            const char *text() const throw();

            //! clone the char with a substituted code
            Char       *subst(const uint8_t data) const;

        private:
            YOCTO_DISABLE_ASSIGN(Char);
            mutable char _txt[4];
        };

    }

}

#endif
