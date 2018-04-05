#include "yocto/program.hpp"
#include "yocto/lang/syntax/parser.hpp"
#include "yocto/lang/syntax/analyzer.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/fs/vfs.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ios/osstream.hpp"
#include "yocto/string/hexdump.hpp"

using namespace yocto;
using namespace Lang;

static inline
void output_style(const size_t len)
{
    for(size_t i=0;i<len;++i)
    {
        std::cerr << "-";
    }
}

YOCTO_PROGRAM_START()
{
    if(argc<=2)
    {
        throw exception("usage: %s grammar.g dir",program);
    }
    const string             gramFile = argv[1];
    const string             gramWork = vfs::to_directory(argv[2]);
    const string             inclExt  = ".inc";
    const string             defsExt  = ".def";

    const size_t length = gramFile.length()+2;
    std::cerr << "  /-----------------------------"; output_style(length); std::cerr << std::endl;
    std::cerr << "  | Dynamo Grammar Compiler for '" << gramFile << "'" << std::endl;
    std::cerr << "  |" << std::endl;
    std::cerr << "  |-Generating..." << std::endl;
    auto_ptr<Syntax::Parser> parser( Syntax::Parser::GenerateFromFile(gramFile) );
    const string tag      = parser->tag; ((string&)tag).to_lower();
    const string inclFile = gramWork + tag + inclExt;
    const string defsFile = gramWork + tag + defsExt;

    {
        std::cerr << "  |-Serializing..." << std::endl;
        string compiled;
        {
            ios::osstream fp(compiled);
            Syntax::Parser::Serialize( Module::OpenFile(gramFile) , fp);
        }

        std::cerr << "  |-Writing Hexdump to '" << inclFile << "'" << std::endl;
        {
            ios::wcstream fp(inclFile);
            hexdump::write(fp,compiled);
        }
    }

    {
        std::cerr << "  |-Writing Defines to '" << defsFile << "'" << std::endl;
        ios::wcstream     fp(defsFile);
        //const string      prefix = parser->tag + '_';
        Syntax::Analyzer  analyzer(*parser);
        analyzer.emitDefinitions(fp,*parser);
    }
    std::cerr << "  |" << std::endl;
    std::cerr << "  \\-----------------------------"; output_style(length); std::cerr << std::endl;

}
YOCTO_PROGRAM_END()
