#include "yocto/program.hpp"
#include "yocto/lang/compiler.hpp"

#include "nwsrt.def"

using namespace yocto;
using namespace Lang;

static const char nwsrt_grammar[] =
{
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

    virtual void onTerminal(const string &label,
                            const int     hCode,
                            const string &content)
    {
        __indent() << "push "; __output(label) << " '" << content << "'" << std::endl;

    }

    virtual void onInternal(const string &label,
                            const int     hCode,
                            const int     nArgs)
    {
        __indent() << "call " << (label) << "/" << nArgs << std::endl;
    }

    private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(TheCompiler);

};


YOCTO_PROGRAM_START()
{
    TheCompiler nws;
    if(argc>1)
    {
        const Module::Handle hModule( new Module(argv[1]) );
        Source               source( hModule );

        nws.ld(source);
    }

}
YOCTO_PROGRAM_END()

