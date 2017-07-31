#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/lang/pattern/joker.hpp"
#include "yocto/lang/pattern/posix.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/ios/graphviz.hpp"

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
        //rx.push_back( RegExp(argv[i],NULL) );
        rx.push_back( RegExp(argv[i],&dict) );
        const string filename = vformat("rx%d.dot",i);
        rx.tail->graphviz(filename);
        std::cerr << "\trendering..." << std::endl;
        ios::graphviz_render(filename);
    }

}
YOCTO_UNIT_TEST_DONE()

