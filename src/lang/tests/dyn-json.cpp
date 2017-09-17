#include "yocto/lang/compiler.hpp"

#include "yocto/program.hpp"
#include "yocto/ios/graphviz.hpp"

using namespace yocto;
using namespace Lang;

static const char code[] =
{
#include "json.inc"
};

#include "json.def"

class JSON_Compiler : public Compiler
{
public:

    explicit JSON_Compiler() : Compiler(Syntax::Parser::Generate(code,sizeof(code)))
    {
    }

    virtual ~JSON_Compiler() throw()
    {
    }


    virtual void initialize()
    {
        std::cerr << "Initializing..." << std::endl;
    }

    virtual void onTerminal(const string &label,
                            const int     hCode,
                            const string &content)
    {
        __indent() << "push "; __output(label) << " '" << content << "'" << std::endl;
        
        switch(hCode)
        {
            case JSON_number: break;
            case JSON_string: break;
            case JSON_null:   break;
            case JSON_true:   break;
            case JSON_false:  break;
            default:
                throw exception("unexpected terminal %s", *label);
        }
    }

    virtual void onInternal(const string &label,
                            const int     hCode,
                            const size_t  nArgs)
    {
        __indent() << "call " << (label) << "/" << nArgs << std::endl;
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
