#include "yocto/ipso/mesh/rectilinear.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace ipso;

YOCTO_UNIT_TEST_IMPL(mesh)
{
    if(argc<=1)
    {
        throw exception("usage: %s DIMS", argv[0]);
    }
    const coord3D dims   = __coord_parser::get<coord3D>(argv[1],"dims");

    std::cerr << "dims   = " << dims   << std::endl;

    {
        axis_names    names("x");
        const patch1D region(1,dims.x);
        rectilinear_mesh<float,1> rmesh( names, region );
        
    }

}
YOCTO_UNIT_TEST_DONE()


