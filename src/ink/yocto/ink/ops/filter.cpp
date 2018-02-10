#include "yocto/ink/ops/filter.hpp"

namespace yocto
{
    namespace Ink
    {
        Filter:: ~Filter() throw()
        {
        }

        Filter:: Filter() throw(): tgt(0), src(0), pfn(0) {}
    }
}
