#include "yocto/lingua/prv/xgen-cpp.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/ios/graphviz.hpp"

using namespace yocto;
using namespace lingua;

YOCTO_UNIT_TEST_IMPL(cpp)
{
#if 0
    xgen_cpp      cpp;
    ios::icstream fp( ios::cstdin );
    source        src(fp);
    lexeme       *lx = 0;
    while( 0 != (lx=cpp.get(src)) )
    {
        std::cerr << lx->label <<  "='" << *lx << "'" << std::endl;
        delete lx;
    }
#endif
    
    xgen_cpp      cpp;
    ios::icstream fp( ios::cstdin );
    l_list        lxm;
    cpp.process(fp,lxm);
    
   
}
YOCTO_UNIT_TEST_DONE()
