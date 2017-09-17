#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/lang/syntax/analyzer.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/sequence/vector.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_UNIT_TEST_IMPL(dyn)
{
    vfs           &fs = local_fs::instance();

    if(argc>1)
    {
        // create it
        const string             parserFile = argv[1];
        auto_ptr<Syntax::Parser> parser( Syntax::Parser::Generate(parserFile,true) );
        {
            ios::wcstream fp("tmp.g");
            Syntax::Parser::Encode(parserFile,fp);
        }

        

        // open stdio
        const Module::Handle hModule( new Module() );
        Source               source(hModule);


        
        // clean output
        const string parserOutDot = parser->tag + "_out.dot";
        const string parserOutGfx = parser->tag + "_out.png";
        fs.try_remove_file(parserOutDot);
        fs.try_remove_file(parserOutGfx);


        // parse
        (std::cerr << "Ready..." << std::endl ).flush();

        auto_ptr<Syntax::Node> tree( parser->parse(source) );

        (std::cerr << "...done" << std::endl ).flush();

        // save tree
        Syntax::Analyzer analyzer( *parser );

        if( tree.is_valid() )
        {
            tree->graphviz(parserOutDot);
            ios::graphviz_render(parserOutDot);
            std::cerr << "Walking..." << std::endl;

            ios::ocstream fp( ios::cstderr );
            analyzer.walk(tree.__get());
        }

    }

}
YOCTO_UNIT_TEST_DONE()
