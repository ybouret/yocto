#include "yocto/pack/xbwt.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(xbwt)
{


    vector<char> input;
    {

        ios::icstream fp( ios::cstdin );
        char C = 0;
        while( fp.query(C) )
        {
            input.push_back(C);
        }
    }
    const size_t size = input.size();
    vector<char>   output(size);
    vector<char>   decode(size);
    const size_t pidx = pack::xbwt::encode(output(), input(), size );
    std::cerr << "pidx=" << pidx << std::endl;

    {
        ios::wcstream fp("from.dat");
        fp << input;
    }

    {
        ios::wcstream fp("xbwt.dat");
        fp << output;
    }

    pack::xbwt::decode(decode(), output(), size,pidx);
    {
        ios::wcstream fp("xdec.dat");
        fp << decode;
    }

    for(size_t i=decode.size();i>0;--i)
    {
        if(decode[i]!=input[i])
        {
            throw exception("XBWT error");
        }
    }
}
YOCTO_UNIT_TEST_DONE()
