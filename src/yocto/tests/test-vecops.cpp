#include "yocto/math/vecops.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

namespace {

    typedef int          coord1D;
    typedef point2d<int> coord2D;
    typedef point3d<int> coord3D;

    YOCTO_VECOPS(__coord,int,COORD)

    

#define TEST_OP2(OP) std::cerr << #OP << "\t=> " << __coord_##OP(lhs,rhs) << std::endl
#define TEST_OP1(OP) std::cerr << #OP << "\t=> " << __coord_##OP(lhs) << std::endl

    template <typename COORD>
    static inline void test_ops(const COORD &lhs, const COORD &rhs)
    {
        std::cerr << std::endl;
        std::cerr << "DIM = " << YOCTO_VECOPS_DIM(coord1D,COORD) << std::endl;
        std::cerr << "lhs=" << lhs << ", rhs=" << rhs << std::endl;
        TEST_OP2(eq);
        TEST_OP1(sum);
        TEST_OP1(prod);
        TEST_OP1(min);
        TEST_OP1(max);
        COORD d(lhs);
        __coord_dec(d);
        std::cerr << "dec" << "\t=>" << d << std::endl;
    }


}
YOCTO_UNIT_TEST_IMPL(vecops)
{
    {
        coord1D u(1);
        coord1D v(2);
        test_ops(u,v);
    }

    {
        coord2D u(2,3);
        coord2D v(4,5);
        test_ops(u,v);
    }

    {
        coord3D u(2,3,4);
        coord3D v(5,6,7);
        test_ops(u,v);
    }
}
YOCTO_UNIT_TEST_DONE()

