#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/lang/lexical/scanner.hpp"
#include "yocto/utest/run.hpp"


using namespace yocto;
using namespace Lang;

class myScanner : public Lexical::Scanner
{
public:

    explicit myScanner() : Lexical::Scanner("myScanner")
    {
        define("ENDL", RegExp("[:endl:]",NULL),  this, & myScanner::onENDL );
        define("WORD", RegExp("[:word:]+",NULL), this, & myScanner::onWORD );
        define("BLANKS", RegExp("[:blanks:]",NULL), this, & myScanner::onBLANKS);


    }

    virtual ~myScanner() throw()
    {
    }

    inline Lexical::ActionType onENDL(const Token &) throw()
    {

        return Lexical::ActionRegular;
    }

    inline Lexical::Action onWORD(const Token &t)
    {
        return Lexical::ActionRegular;
    }

    inline Lexical::Action onBLANKS(const Token &t)
    {
        return Lexical::ActionRegular;
    }


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(myScanner);

};

YOCTO_UNIT_TEST_IMPL(scan)
{
    
    Module::Handle hModule( new Module() ); // open stdio
    Source         source( hModule );       // register it in source

    Lexical::Scanner scanner("main.scanner");


    Lexical::Units lexemes;

    do
    {
        Lexical::Unit             *lex  = 0;
        const Lexical::ActionType  kind = scanner.probe(source, lex);
        (void)kind;
        if(!lex) break;
        lexemes.push_back(lex);
    }
    while(true);

}
YOCTO_UNIT_TEST_DONE()
