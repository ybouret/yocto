#include "yocto/program.hpp"
#include "yocto/lang/compiler.hpp"

#include "nwsrt.def"

using namespace yocto;
using namespace Lang;

static const char nwsrt_grammar[] = {
#include "nwsrt.inc"
};

class TheCompiler : public Compiler
{
    public:

    explicit TheCompiler() : Compiler(Syntax::Parser::Generate(nwsrt_grammar,sizeof(nwsrt_grammar)))
    {
    }

    virtual ~TheCompiler() throw()
    {
    }

};


YOCTO_PROGRAM_START()
{
    TheCompiler nws;
    const Module::Handle hModule( new Module() );
    Source               source( hModule );

    nws.ld(source);

}
YOCTO_PROGRAM_END()

