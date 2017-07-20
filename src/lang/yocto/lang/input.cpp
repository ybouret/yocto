#include "yocto/lang/input.hpp"

namespace yocto
{
    namespace lang
    {
        input:: ~input() throw()
        {
            
        }

        input:: input(const string &tag, const module &handle) :
        in(handle),
        line(1),
        name(tag)
        {

        }

    }
}
