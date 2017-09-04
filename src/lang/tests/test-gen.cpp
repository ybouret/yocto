#include "yocto/lang/gen/gcompiler.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_UNIT_TEST_IMPL(gen)
{
    vfs           &fs = local_fs::instance();

    fs.try_remove_file("tree.dot");
    fs.try_remove_file("tree.png");
    fs.try_remove_file("gram.dot");
    fs.try_remove_file("gram.png");

    Module::Handle     hm( new Module() );
    Source             source( hm );
    Syntax::gCompiler &gen = Syntax::gCompiler::instance();

    gen.getAST.graphviz("gram.dot");
    ios::graphviz_render("gram.dot");

    auto_ptr<Syntax::Node> tree( gen.getAST(source) );
    if( tree.is_valid() )
    {
        tree->graphviz("tree.dot");
        ios::graphviz_render("tree.dot");
    }
}
YOCTO_UNIT_TEST_DONE()
