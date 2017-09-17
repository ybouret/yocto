#include "yocto/program.hpp"
#include "yocto/lang/syntax/parser.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/fs/vfs.hpp"
#include "yocto/ios/ocstream.hpp"

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

    auto_ptr<Syntax::Parser> parser( Syntax::Parser::Generate(gramFile) );
    const string inclFile = gramWork + parser->tag + inclExt;
    std::cerr << "Saving into " << inclFile << std::endl;
    {
        ios::wcstream fp(inclFile);
        Syntax::Parser::Encode(gramFile,fp);
    }

}
YOCTO_PROGRAM_END()
