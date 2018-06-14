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
        while(io.size()>=8)
        {
            gp.write( io.pop_full<uint8_t>() );
            obits += 8;
        }
    }
    alpha.display();
    io.fill_to_byte_with(false);
    while(io.size()>=8)
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

#include "yocto/hashing/sha1.hpp"
YOCTO_UNIT_TEST_IMPL(huffio)
{
    std::cerr << "Huffman I/O" << std::endl;
    hashing::sha1     H;
    ios::bitio        io;
    H.set();
    size_t ibytes = 0;
    char C=0;
    {
        std::cerr << "ihuff..." << std::endl;
        Huffman::Alphabet ihuff;
        ios::icstream     fp( ios::cstdin );
        while( fp.query(C) )
        {
            ihuff.encode(io,C);
            H.run_type(C);
            ++ibytes;
        }
        ihuff.flush(io);
    }
    assert(0==(io.size()%8));
    const size_t   obytes = io.size()>>3;
    const uint64_t i64    = H.key<uint64_t>();
    std::cerr << "ibytes = " << ibytes << " => obytes=" << obytes << std::endl;
    
    {
        H.set();
        Huffman::Alphabet ohuff;
        Huffman::Alphabet::DecodeContext ctx;
        ios::ocstream fp( ios::cstdout );
        while( ohuff.decode(io,ctx,C) )
        {
            //fp.write(C);
            H.run_type(C);
        }
    }
    const uint64_t o64 = H.key<uint64_t>();
    std::cerr << std::hex << "o64=" << o64 << "/" << i64 << std::endl;

}
YOCTO_UNIT_TEST_DONE()

