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

    auto_ptr<Syntax::Parser> parser( Syntax::Parser::GenerateFromFile(gramFile) );
    const string tag      = parser->tag; ((string&)tag).to_lower();
    const string inclFile = gramWork + tag + inclExt;
    const string defsFile = gramWork + tag + defsExt;
    //std::cerr << "code in " << inclFile << std::endl;
    //std::cerr << "defs in " << defsFile << std::endl;

    {
        string compiled;
        {
            ios::osstream fp(compiled);
            Syntax::Parser::Serialize( Module::OpenFile(gramFile) , fp);
        }

        {
            ios::wcstream fp(inclFile);
            hexdump::write(fp,compiled);
        }
    }

    {
        ios::wcstream     fp(defsFile);
        const string      prefix = parser->tag + '_';
        Syntax::Analyzer  analyzer(*parser);
        analyzer.emitDefinitions(fp,prefix);
    }

}
YOCTO_PROGRAM_END()
