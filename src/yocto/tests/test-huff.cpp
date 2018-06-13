#include "yocto/pack/huffman.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/icstream.hpp"


using namespace yocto;
using namespace pack;

YOCTO_UNIT_TEST_IMPL(huff)
{
    std::cerr << "Huffman::NumBytes=" << Huffman::NumBytes << std::endl;
    std::cerr << "Huffman::NYT     =" << Huffman::NYT      << std::endl;
    std::cerr << "Huffman::EOS     =" << Huffman::EOS      << std::endl;
    std::cerr << "Huffman::NumChars=" << Huffman::NumChars << std::endl;

    Huffman::Alphabet alpha;
    alpha.display();
    char C=0;
    ios::icstream fp( ios::cstdin );
    while( fp.query(C) )
    {
        alpha.add(C);
    }
    alpha.display();
    alpha.buildTree();
}
YOCTO_UNIT_TEST_DONE()


