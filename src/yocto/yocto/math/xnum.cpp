#include "yocto/xnumeric.hpp"
#include "yocto/math/types.hpp"
#include "yocto/math/types.hxx"


namespace yocto
{

    template <>
    float xnumeric<float>::minimum() { return math::numeric<float>::minimum; }

    template <>
    double xnumeric<double>::minimum() { return math::numeric<double>::minimum; }

}


