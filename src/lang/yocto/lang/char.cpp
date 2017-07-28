#include "yocto/lang/char.hpp"

namespace yocto
{

    namespace Lang
    {

        Char::Char(const CharInfo &info, const uint8_t data) throw() :
        CharInfo(info),
        prev(0),
        next(0),
        code(data)
        {
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
        }


    }

}
