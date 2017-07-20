#include "yocto/lang/t-char.hpp"

namespace yocto
{
    namespace lang
    {

        t_char:: ~t_char() throw()
        {
        }

        t_char::t_char(const input::pointer &in,
                       const uint8_t         ch,
                       const int             il) throw() :
        next(0),
        prev(0),
        from(in),
        code(ch),
        line(il)
        {
        }

        t_char:: t_char( const t_char &other ) throw() :
        next(0),
        prev(0),
        from(other.from),
        code(other.code),
        line(other.line)
        {
        }
        
    }

}

