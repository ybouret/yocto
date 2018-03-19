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
        std::cerr << "-- Creating Parser" << std::endl;
        const string             parserFile = argv[1];
        auto_ptr<Syntax::Parser> parser( Syntax::Parser::GenerateFromFile(parserFile,true) );

        {
            std::cerr << "\t-- Encoding..." << std::endl;
            ios::wcstream fp("encoded.dat");
            Syntax::Parser::Encode(parserFile,fp);
        }

        {
            std::cerr << "\t-- Writing def" << std::endl;
            ios::wcstream     fp("def_hxx.dat");
            const string      prefix = parser->tag + '_';
            Syntax::Analyzer  analyzer(*parser);
            analyzer.emitDefinitions(fp,prefix);
        }

        {
            std::cerr << "\t-- Compiling" << std::endl;
            Syntax::DynamoCompiler dyn;
            {
                ios::wcstream          fp("compiled.bin");
                dyn.Compile( Module::OpenFile(argv[1]) , fp);
            }

            {
                std::cerr << "\t-- Reloading" << std::endl;
                Syntax::Node *tree = Syntax::Node::loadFrom( Module::OpenFile("compiled.bin"), dyn);
                auto_ptr<Syntax::Parser> reloaded ( dyn.encode(tree) );
                std::cerr << "\t\treloaded!" << std::endl;
                reloaded.release();
            }
        }

        // open stdio
        std::cerr << "-- Reading input from STDIN" << std::endl;
        Source               source(Module::OpenSTDIN());
        
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
        std::cerr << "-- Runing Analyzer" << std::endl;
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
