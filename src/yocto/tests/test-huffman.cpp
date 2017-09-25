#include "yocto/pack/huffman.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/pack/mtf.hpp"

using namespace yocto;
using namespace pack;

YOCTO_UNIT_TEST_IMPL(huff)
{
    std::cerr << "sizeof(Huffman::Alphabet)=" << sizeof(Huffman::Alphabet) << std::endl;
    Huffman::AlphaEncoder alpha0;
    Huffman::AlphaEncoder alpha1;
    move_to_front         mtf;

    ios::icstream fp(ios::cstdin);
    ios::wcstream op0("huff0.dat");
    ios::wcstream op1("huff1.dat");

    ios::bitio    bio0;
    ios::bitio    bio1;

    size_t icount=0,ocount0=0,ocount1=0;

    char C = 0;
    while( fp.query(C) )
    {
        ++icount;
        alpha0.encode(C,bio0);
        while(bio0.size()>=8)
        {
            const uint8_t B = bio0.pop_full<uint8_t>();
            op0.write(B);
            ++ocount0;
        }

        alpha1.encode(mtf.encode(C),bio1);
        while(bio1.size()>=8)
        {
            const uint8_t B = bio1.pop_full<uint8_t>();
            op1.write(B);
            ++ocount1;
        }

    }
    bio0.fill_to_byte_with(false);
    while(bio0.size()>=8)
    {
        op0.write( bio0.pop_full<uint8_t>() );
        ++ocount0;
    }

    bio1.fill_to_byte_with(false);
    while(bio1.size()>=8)
    {
        op1.write( bio1.pop_full<uint8_t>() );
        ++ocount1;
    }

    std::cerr << "icount=" << icount << " => " << ocount0 << " | " << ocount1 << std::endl;

}
YOCTO_UNIT_TEST_DONE()


