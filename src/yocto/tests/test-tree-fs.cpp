#include "yocto/fs/tree-fs.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

#define __SHOW(TYPE) std::cerr << "sizeof(" << #TYPE << ") = " << sizeof(TYPE) << std::endl
YOCTO_UNIT_TEST_IMPL(TreeFS)
{
    __SHOW(TreeFS::Node);
    __SHOW(TreeFS::Nodes);
    __SHOW(vslot);

}
YOCTO_UNIT_TEST_DONE()

