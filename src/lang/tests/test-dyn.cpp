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
        // open stdio
        const Module::Handle hModule( new Module() );
        Source               source(hModule);


        Lexical::Units prefetch;
        if(argc>2)
        {
            const string arg = argv[2];
            if("prefetch"==arg)
            {
                std::cerr << "\t\tPREFETCH" << std::endl;
                parser->getAll(prefetch,source);
                parser->unget_copy_of(prefetch);
            }
        }

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

        std::cerr << "#prefetch=" << prefetch.size << std::endl;
        for(const Lexical::Unit *u=prefetch.head;u;u=u->next)
        {
            std::cerr << u->label << " : " << *u << std::endl;
        }
    }

}
YOCTO_UNIT_TEST_DONE()
