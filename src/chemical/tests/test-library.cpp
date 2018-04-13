#include "yocto/chemical/library.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;
using namespace chemical;

YOCTO_UNIT_TEST_IMPL(library)
{
    library::pointer lib( new library("lib") );
    species &H  = lib->add("H+",1);
    species &OH = lib->add("HO-",-1);
    std::cerr << H.name  << "," << H.z  << std::endl;
    std::cerr << OH.name << "," << OH.z << std::endl;

    std::cerr << lib << std::endl;
}
YOCTO_UNIT_TEST_DONE()

