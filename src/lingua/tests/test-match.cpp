#include "yocto/lingua/pattern/matcher.hpp"
#include "yocto/lingua/pattern/regexp.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/lingua/pattern/posix.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ios/imstream.hpp"

using namespace yocto;
using namespace lingua;

YOCTO_UNIT_TEST_IMPL(match)
{

    if(argc>1)
    {
        const string      expr = argv[1];
        matcher           m(regexp(expr,NULL));
        string            s;

        ios::icstream fp( ios::cstdin );
        while( s.clear(), (std::cerr << "> ").flush(), fp.gets(s) )
        {
            std::cerr << "'" << expr << "' matching '" <<  s << "'" << std::endl;
            std::cerr << "|_full: ";
            if( m.full_match(s) )
            {
                std::cerr << "TRUE" << std::endl;
            }
            else
            {
                std::cerr << "FALSE" << std::endl;
            }

            std::cerr << "|_partial: ";
            if(m.partial_match(s))
            {
                std::cerr << "TRUE: '" << m.last << "'" << std::endl;
            }
            else
            {
                std::cerr << "FALSE" << std::endl;
            }
        }

    }

}
YOCTO_UNIT_TEST_DONE()

