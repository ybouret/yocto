#include "yocto/lang/char.hpp"
#include "yocto/code/utils.hpp"
#include <cstring>

namespace yocto
{

    namespace Lang
    {

#define Y_CHAR_INIT() memset(_txt,0,sizeof(_txt))

        Char::Char(const CharInfo &info, const uint8_t data) throw() :
        CharInfo(info),
        prev(0),
        next(0),
        code(data),
        _txt()
        {
            Y_CHAR_INIT();
        }

        Char:: ~Char() throw()
        {
        }

        Char:: Char(const Char &other) throw() :
        yocto::Lang::CharInfo(other),
        prev(0),
        next(0),
        code(other.code)
        {
            Y_CHAR_INIT();
        }

        const char * Char:: text() const throw()
        {
            Y_CHAR_INIT();
            if(code>=32&&code<127)
            {
                _txt[0] = code;
            }
            else
            {
                _txt[0] = 'x';
                memcpy( &_txt[1], hexa_text_lower[code], 2);
            }
            return _txt;
        }


    }

}

