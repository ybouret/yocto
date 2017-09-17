#include "yocto/lang/compiler.hpp"

#include "yocto/program.hpp"
#include "yocto/ios/graphviz.hpp"

using namespace yocto;
using namespace Lang;

static const char code[] =
{
#include "json.inc"
};


class JSON_Compiler : public Compiler
{
public:
    explicit JSON_Compiler() : Compiler(Syntax::Parser::Generate(code,sizeof(code)))
    {
    }

    virtual ~JSON_Compiler() throw()
    {
    }


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(JSON_Compiler);
};

YOCTO_PROGRAM_START()
{

    JSON_Compiler jCompiler;

    const Module::Handle hModule( new Module() );
    Source               source( hModule );

    jCompiler.ld(source);


}
YOCTO_PROGRAM_END()
