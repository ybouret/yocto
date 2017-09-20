#include "yocto/pack/esf.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/icstream.hpp"

using namespace yocto;
using namespace pack;

YOCTO_UNIT_TEST_IMPL(esf)
{
    ESF::Alphabet alpha;

    ios::icstream fp( ios::cstdin );
    char C;
    while( fp.query(C) )
    {
        alpha.increase(uint8_t(C));
        //alpha.display_items();
        alpha.display_ilist();
    }

}
YOCTO_UNIT_TEST_DONE()

