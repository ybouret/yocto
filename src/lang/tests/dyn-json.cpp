#include "yocto/lang/syntax/parser.hpp"
#include "yocto/program.hpp"
#include "yocto/ios/graphviz.hpp"

using namespace yocto;
using namespace Lang;

static const char code[] =
{
#include "json.inc"
};

YOCTO_PROGRAM_START()
{
    std::cerr << "Building Parser, code.size=" << sizeof(code) << std::endl;
    auto_ptr<Syntax::Parser> jParser( Syntax::Parser::Generate(code,sizeof(code)) );
    std::cerr << "..done" << std::endl;
    if(true)
    {
        jParser->graphviz("json.dot");
        ios::graphviz_render("json.dot");
    }
}
YOCTO_PROGRAM_END()
