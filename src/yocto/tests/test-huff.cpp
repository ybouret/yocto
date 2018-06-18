#include "yocto/pack/huffman.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/ios/ocstream.hpp"


using namespace yocto;
using namespace pack;

YOCTO_UNIT_TEST_IMPL(huffenc)
{
    Huffman::Encoder huffenc;

    ios::icstream fp( ios::cstdin  );
    ios::ocstream gp( ios::cstdout );

    size_t isize = 0;
    size_t osize = 0;
    char C = 0;
    while( fp.query(C) )
    {
        ++isize;
        huffenc.write(C);
        while(huffenc.query(C))
        {
            gp.write(C);
            ++osize;
        }
    }
    huffenc.flush();
    while(huffenc.query(C))
    {
        gp.write(C);
        ++osize;
    }

    std::cerr << isize << " => " << osize << std::endl;


}
YOCTO_UNIT_TEST_DONE()

YOCTO_UNIT_TEST_IMPL(huffdec)
{
    Huffman::Decoder huffdec;

    ios::icstream fp( ios::cstdin  );
    ios::ocstream gp( ios::cstdout );
    char C = 0;
    while( fp.query(C) )
    {
        huffdec.write(C);
        while(huffdec.query(C))
        {
            gp.write(C);
        }
    }
    huffdec.flush();
    while(huffdec.query(C))
    {
        gp.write(C);
    }
}
YOCTO_UNIT_TEST_DONE()

