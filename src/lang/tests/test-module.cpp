#include "yocto/lang/module.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

using namespace Lang;

YOCTO_UNIT_TEST_IMPL(module)
{
    if(argc>1)
    {
        Module::Handle hModule( new Module(argv[1]) );
        Char *ch = 0;
        while( 0 != (ch=hModule->get()) )
        {
            const char C = ch->code;
            delete ch;
            std::cerr << C;
        }
        std::cerr << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()

