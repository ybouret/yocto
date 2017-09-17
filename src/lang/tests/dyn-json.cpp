#include "yocto/lang/syntax/parser.hpp"
#include "yocto/lang/syntax/analyzer.hpp"

#include "yocto/program.hpp"
#include "yocto/ios/graphviz.hpp"

using namespace yocto;
using namespace Lang;

static const char code[] =
{
#include "json.inc"
};

#if 0
class JSON_Analyzer : public Syntax::Analyzer
{
public:
    explicit JSON_Analyzer() {}
    virtual ~JSON_Analyzer() throw() {}
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(JSON_Analyzer);
};
#endif

YOCTO_PROGRAM_START()
{
    std::cerr << "Building Parser, code.size=" << sizeof(code) << std::endl;
    auto_ptr<Syntax::Parser> jParser( Syntax::Parser::Generate(code,sizeof(code)) );
    std::cerr << "..done" << std::endl;

    if(false)
    {
        jParser->graphviz("json.dot");
        ios::graphviz_render("json.dot");
    }

    const Module::Handle hModule( new Module() );
    Source               source( hModule );

    auto_ptr<Syntax::Node> jTree( jParser->parse(source) );
    if(!jTree.is_valid())
    {
        throw exception("unexpected empty tree");
    }

    Syntax::Analyzer jAnalyzer( *jParser );
    jAnalyzer.walk( jTree.__get() );
    

}
YOCTO_PROGRAM_END()
