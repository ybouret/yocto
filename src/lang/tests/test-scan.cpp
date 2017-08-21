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
        define("ENDL",   RegExp("[:endl:]",NULL),   this, & myScanner::onENDL );
        define("WORD",   RegExp("[:word:]+",NULL),  this, & myScanner::onWORD );
        define("BLANKS", RegExp("[:blank:]+",NULL), this, & myScanner::onBLANKS);
        define("PUNCT",  RegExp("[:punct:]+",NULL), this, & myScanner::onPUNCT);

    }

    virtual ~myScanner() throw()
    {
    }

    inline bool onENDL(const Token &) throw()
    {
        if(module) module->newLine();
        return true;
    }

    inline bool onWORD(const Token &t)
    {
        return true;
    }

    inline bool onBLANKS(const Token &t)
    {
        return true;
    }

    inline bool onPUNCT(const Token &t)
    {
        return true;
    }


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(myScanner);

};

YOCTO_UNIT_TEST_IMPL(scan)
{
    
    Module::Handle hModule( new Module() ); // open stdio
    Source         source( hModule );       // register it in source

    myScanner scanner;
    std::cerr << "\tmax_label_length=" << scanner.max_label_length << std::endl;

    Lexical::Units lexemes;

    do
    {
        bool           reg  = true;
        Lexical::Unit *lex  = scanner.probe(source,reg);
        if(!lex) break;
        lexemes.push_back(lex);
    }
    while(true);

    for(const Lexical::Unit *u = lexemes.head; u; u=u->next )
    {
        std::cerr << u->label;
        for(size_t i=u->label.length();i<=scanner.max_label_length;++i)
        {
            std::cerr << ' ';
        }
        std::cerr << "'" << *u  << "'";
        std::cerr << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()
