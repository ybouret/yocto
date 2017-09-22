#include "yocto/lang/pattern/matching.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/icstream.hpp"

using namespace yocto;
using namespace Lang;


YOCTO_UNIT_TEST_IMPL(match)
{
    if(argc>1)
    {
        std::cerr << "Compiling Matching..." << std::endl;
        Matching      matching = argv[1];
        ios::icstream fp( ios::cstdin );
        std::cerr << "Ready>" << std::endl; std::cerr.flush();
        string line;
        while( fp.gets(line) )
        {
            //std::cerr << line << std::endl;
            if(matching.completely(line))
            {
                std::cerr << "+" << line << std::endl;
            }

            if(matching.partially(line))
            {
                std::cerr << "*" << line << std::endl;
            }
        }

    }
}
YOCTO_UNIT_TEST_DONE()

