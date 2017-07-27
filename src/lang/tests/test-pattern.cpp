#include "yocto/lang/pattern/basic.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

using namespace Lang;

YOCTO_UNIT_TEST_IMPL(pattern)
{
    if(argc>1)
    {
        Module::Handle hModule( new Module(argv[1]) );
        Source         source( hModule );
        source.prefetch(10);

        Patterns patterns;
        patterns.push_back( new Any1()      );
        patterns.push_back( new Single('A') );

        Patterns patterns2(patterns);

    }
}
YOCTO_UNIT_TEST_DONE()

