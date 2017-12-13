#include "yocto/lang/source.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

using namespace Lang;

YOCTO_UNIT_TEST_IMPL(source)
{
    if(argc>1)
    {
        Source         source( Module::OpenFile(argv[1] ) );
        source.prefetch(10);
        Char *ch = 0;
        while( 0 != (ch=source.query()) )
        {
            std::cerr << ch->line << "." << ch->column << ": '" << ch->text() << "'" << std::endl;
            delete ch;
        }
    }
}
YOCTO_UNIT_TEST_DONE()

