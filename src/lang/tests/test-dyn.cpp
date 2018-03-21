#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/lang/syntax/analyzer.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/ios/file-loader.hpp"
#include "yocto/ios/osstream.hpp"
#include "yocto/string/hexdump.hpp"

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

#if 0
        {
            std::cerr << "\t-- Encoding..." << std::endl;
            ios::wcstream fp("encoded.dat");
            Syntax::Parser::Encode(parserFile,fp);
        }
#endif
        
        {
            std::cerr << "\t-- Writing def" << std::endl;
            ios::wcstream     fp("def_hxx.dat");
            Syntax::Analyzer  analyzer(*parser);
            analyzer.emitDefinitions(fp,*parser);
        }

        {
            std::cerr << "\t-- Compiling" << std::endl;
            Syntax::DynamoCompiler dyn;
            {
                ios::wcstream          fp("serialized.bin");
                dyn.Serialize( Module::OpenFile(argv[1]) , fp);
            }

            {
                std::cerr << "\t-- Reloading" << std::endl;
                Syntax::Node *tree = Syntax::Node::loadFrom( Module::OpenFile("serialized.bin"), dyn);
                auto_ptr<Syntax::Parser> reloaded ( dyn.encode(tree) );
                std::cerr << "\t\treloaded!" << std::endl;
                reloaded.release();
            }

            {
                auto_ptr<Syntax::Parser> reloaded( Syntax::Parser::CompileFile("serialized.bin") );
                std::cerr << "\t\treloaded again!" << std::endl;
                reloaded.release();
            }

            {
                ios::file_content          content;
                ios::file_loader::append_to(content, "serialized.bin" );
                auto_ptr<Syntax::Parser> reloaded( Syntax::Parser::CompileData("serialized",(char *)content.rw(),content.length()));
                std::cerr << "\t\treloaded once more!" << std::endl;
                reloaded.release();
            }

            {
                string data;
                {
                    ios::osstream fp(data);
                    dyn.Serialize( Module::OpenFile(argv[1]) , fp);
                }
                {
                    ios::wcstream fp("serialized.dat");
                    hexdump::write(fp,data);
                }
                std::cerr << "\t\tand serialized as a C array of " << data.size() << " bytes" << std::endl;
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
