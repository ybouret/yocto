#include "yocto/fs/tree-fs.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(TreeFS)
{
    const string rootPath = "/";
    std::cerr << "rootPath=" << rootPath << std::endl;
    const string rdirPath = vfs::to_directory(rootPath);
    std::cerr << "rdirPath=" << rdirPath << std::endl;
    
    TreeFS<int> tfs;
}
YOCTO_UNIT_TEST_DONE()

