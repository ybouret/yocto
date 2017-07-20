#include "yocto/lang/input.hpp"

namespace yocto
{
    namespace lang
    {
        input:: ~input() throw()
        {
            
        }

        input:: input(const module &h,
                      const string &n) :
        handle(h),
        name(n)
        {

        }

    }
}
