#include "yocto/lang/gen/gcompiler.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_UNIT_TEST_IMPL(gen)
{
    std::cerr << "sizeof(Syntax::gParser  )=" << sizeof(Syntax::gParser)   << std::endl;
    std::cerr << "sizeof(Syntax::gCompiler)=" << sizeof(Syntax::gCompiler) << std::endl;
    
    vfs           &fs = local_fs::instance();

    fs.try_remove_file("tree.dot");
    fs.try_remove_file("tree.png");
    fs.try_remove_file("gram.dot");
    fs.try_remove_file("gram.png");

    Module::Handle     hm( new Module() );
    Source             source( hm );
    Syntax::gCompiler  gen;
    
    gen.verbose = true;
    gen.getAST.graphviz("gram.dot");
    ios::graphviz_render("gram.dot");

    auto_ptr<Syntax::Node> tree( gen.getAST(source) );
    
    assert(tree.is_valid());
    tree->graphviz("tree.dot");
    ios::graphviz_render("tree.dot");

    auto_ptr<Syntax::Parser> P( gen.encode(tree.__get()) );

}
YOCTO_UNIT_TEST_DONE()
