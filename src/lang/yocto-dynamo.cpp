#include "yocto/program.hpp"
#include "yocto/lang/syntax/parser.hpp"
#include "yocto/lang/syntax/analyzer.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/fs/vfs.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/sequence/vector.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_PROGRAM_START()
{
    if(argc<=2)
    {
        throw exception("usage: %s grammar.g dir",program);
    }
    const string             gramFile = argv[1];
    const string             gramWork = argv[2];
    vfs::as_directory((string&)gramWork);
    const string             inclExt  = ".inc";
    const string             defsExt  = ".def";

    auto_ptr<Syntax::Parser> parser( Syntax::Parser::Generate(gramFile) );
    const string tag      = parser->tag; ((string&)tag).to_lower();
    const string inclFile = gramWork + tag + inclExt;
    const string defsFile = gramWork + tag + defsExt;
    std::cerr << "code in " << inclFile << std::endl;
    std::cerr << "defs in " << defsFile << std::endl;

    {
        ios::wcstream fp(inclFile);
        Syntax::Parser::Encode(gramFile,fp);
    }

    {
        const string   prefix = parser->tag + '_';
        vector<string> terms;
        vector<string> rules;
        parser->collectLabels(terms,rules);
        ios::wcstream fp(defsFile);
        fp << "/* #Terminals */\n";
        Hasher::emit_defines(fp,terms,prefix,0);
        fp << "/* #Internals */\n";
        Hasher::emit_defines(fp,rules,prefix,Syntax::Analyzer::RulesShift);

    }

}
YOCTO_PROGRAM_END()
