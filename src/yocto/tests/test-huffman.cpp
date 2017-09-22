#include "yocto/pack/huffman.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/icstream.hpp"

using namespace yocto;
using namespace pack;

YOCTO_UNIT_TEST_IMPL(huff)
{
    std::cerr << "MaxNodes=" << Huffman::MaxNodes << std::endl;
    Huffman::Alphabet alpha;
    ios::icstream fp(ios::cstdin);
    char C = 0;
    while( fp.query(C) )
    {
        alpha.increase(C);
    }
    alpha.display();
    alpha.build_tree();
    alpha.graph_tree();
    alpha.display();


    if(false)
    {
        alpha.rescale();
        alpha.display();
    }
}
YOCTO_UNIT_TEST_DONE()


