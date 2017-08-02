#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/lang/pattern/joker.hpp"
#include "yocto/lang/pattern/posix.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_UNIT_TEST_IMPL(regexp)
{
    Patterns    rx;
    PatternDict dict;
    dict.define("INT", OneOrMore( posix::digit() ) );
    for(int i=1;i<argc;++i)
    {
        std::cerr << "Compiling '" << argv[i] << "'" << std::endl;
        rx.push_back( RegExp(argv[i],&dict) );
        const string filename = vformat("rx%d.dot",i);
        rx.tail->graphviz(filename);
        std::cerr << "\trendering..." << std::endl;
        ios::graphviz_render(filename);

        {
            const string  cname = vformat("rx%d.bin",i);
            {
                ios::wcstream fp(cname);
                rx.tail->save(fp);
            }

        }

        {
            const string pCode = rx.tail->toString();
            std::cerr << "pCode=" << pCode << std::endl;
        }

    }

}
YOCTO_UNIT_TEST_DONE()

