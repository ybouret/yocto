#ifndef YOCTO_LANG_T_CHAR_INCLUDED
#define YOCTO_LANG_T_CHAR_INCLUDED 1

#include "yocto/lang/input.hpp"

namespace yocto
{
    namespace lang
    {
        class t_char : public object
        {
        public:
            t_char              *next;
            t_char              *prev;
            const input::pointer from;
            uint8_t              code;
            int                  line;

            t_char(const input::pointer &in,
                   const uint8_t         ch = 0,
                   const int             il = 1) throw();

            virtual ~t_char() throw();

            t_char(const t_char &other) throw();

        private:
            YOCTO_DISABLE_ASSIGN(t_char);
        };
    }
}


#endif
