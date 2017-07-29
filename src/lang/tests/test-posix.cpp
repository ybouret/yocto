#include "yocto/lang/pattern/posix.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/ios/graphviz.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_UNIT_TEST_IMPL(posix)
{
#define TEST_POSIX(EXPR) do {\
auto_ptr<Pattern> p( posix::EXPR() );\
const string filename = #EXPR ".dot";\
std::cerr << "Rendering " #EXPR  "..." << std::endl; \
p->graphviz(filename);\
ios::graphviz_render(filename);\
} while(false)

    TEST_POSIX(lower);
    TEST_POSIX(upper);
    TEST_POSIX(digit);
    TEST_POSIX(alpha);
    TEST_POSIX(alnum);
    TEST_POSIX(xdigit);
    TEST_POSIX(blank);
    TEST_POSIX(space);
    TEST_POSIX(punct);
    TEST_POSIX(dot);
    TEST_POSIX(word);

}
YOCTO_UNIT_TEST_DONE()
