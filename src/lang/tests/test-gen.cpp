#include "yocto/lang/gen/gcompiler.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_UNIT_TEST_IMPL(gen)
{
    vfs           &fs = local_fs::instance();

    if(argc>1)
    {
        const string             parserFile = argv[1];
        auto_ptr<Syntax::Parser> P( Syntax::Parser::Generate(parserFile,true) );

        const string treeDot = P->tag + "_out.dot";
        const string treeGfx = P->tag + "_out.png";

        fs.try_remove_file(treeDot);
        fs.try_remove_file(treeGfx);

        Module::Handle hm( new Module() );
        Source         source( hm );
        (std::cerr << "Ready..." << std::endl).flush();

        auto_ptr<Syntax::Node> tree( P->parse(source) );

        (std::cerr << "...done" << std::endl).flush();
        if(tree.is_valid())
        {
            tree->graphviz(treeDot);
            ios::graphviz_render(treeDot);
        }
    }

}
YOCTO_UNIT_TEST_DONE()
