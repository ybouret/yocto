#include "yocto/xnumeric.hpp"
#include "yocto/math/types.hpp"

namespace yocto
{
    YOCTO_MATH_DECL(minimum);

    template <>
    float xnumeric<float>::minimum() { return math::numeric<float>::minimum; }

    template <>
    double xnumeric<double>::minimum() { return math::numeric<double>::minimum; }

}


