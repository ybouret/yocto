#include "yocto/pack/huffman.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/ios/ocstream.hpp"


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
    ios::wcstream gp( "huff.dat" );
    ios::bitio io;
    size_t ibits = 0;
    size_t obits = 0;
    while( fp.query(C) )
    {
        alpha.encode(io,C);
        ibits+=8;
        while(io.size()>8)
        {
            gp.write( io.pop_full<uint8_t>() );
            obits += 8;
        }
    }
    alpha.display();
    io.fill_to_byte_with(false);
    while(io.size()>8)
    {
        gp.write( io.pop_full<uint8_t>() );
        obits += 8;
    }
    std::cerr << "ibits=" << ibits << " => " << (ibits>>3) << std::endl;
    std::cerr << "obits=" << obits << " => " << (obits>>3) << std::endl;
    std::cerr << "saving tree..." << std::endl;
    alpha.saveTree();
}
YOCTO_UNIT_TEST_DONE()


