#include "yocto/lang/dynamo/compiler.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_UNIT_TEST_IMPL(dyn)
{
    vfs           &fs = local_fs::instance();
    

    const Module::Handle hModule( new Module() );
    Source               source(hModule);

    Syntax::DynamoCompiler P;
    P.verbose = true;

    (std::cerr << "Ready..." << std::endl ).flush();

    auto_ptr<Syntax::Node> tree( P.parse(source) );
    
    (std::cerr << "...done" << std::endl ).flush();

    if( tree.is_valid() )
    {
        tree->graphviz("tree.dot");
        ios::graphviz_render("tree.dot");
    }

    auto_ptr<Syntax::Parser> parser( P.encode( tree.yield() ) );
    
}
YOCTO_UNIT_TEST_DONE()
