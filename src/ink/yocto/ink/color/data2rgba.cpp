#include "yocto/ink/color/data2rgba.hpp"

namespace yocto
{
    namespace Ink
    {
        data2rgba::  data2rgba() throw() {}
        data2rgba:: ~data2rgba() throw() {}

        RGBA data2rgba:: operator()(const void *addr)
        {
            return get(addr);
        }

    }

}
