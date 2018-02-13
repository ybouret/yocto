
#include "yocto/associative/hlist.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/core/node.hpp"
#include "yocto/string.hpp"
#include "support.hpp"

using namespace yocto;

typedef core::dnode_of<string> dNode;

YOCTO_UNIT_TEST_IMPL(HList)
{
    hlist<int,dNode> hl;
    for(size_t i=0;i<10;++i)
    {
        //const string k = gen<string>::get();
        hl.insert(i, new dNode(i) );
    }
}
YOCTO_UNIT_TEST_DONE()

