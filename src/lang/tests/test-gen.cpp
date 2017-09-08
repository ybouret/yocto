#include "yocto/lang/gen/gcompiler.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_UNIT_TEST_IMPL(gen)
{
    //vfs           &fs = local_fs::instance();

    if(argc>1)
    {
        const string             parserFile = argv[1];
        auto_ptr<Syntax::Parser> P( Syntax::Parser::Generate(parserFile,true) );

    }

}
YOCTO_UNIT_TEST_DONE()
