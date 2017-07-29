#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/ios/graphviz.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_UNIT_TEST_IMPL(regexp)
{
    Patterns    rx;
    PatternDict dict;
    for(int i=1;i<argc;++i)
    {
        rx.push_back( RegExp(argv[i],&dict) );
        const string filename = vformat("rx%d.dot",i);
        rx.tail->graphviz(filename);
        ios::graphviz_render(filename);
    }

}
YOCTO_UNIT_TEST_DONE()

