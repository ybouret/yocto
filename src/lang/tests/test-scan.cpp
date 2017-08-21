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
        define("ENDL",   RegExp("[:endl:]",NULL),         this, & myScanner::onENDL );
        define("INT",    RegExp("[0-9]+",NULL),           this, & myScanner::emit);
        define("DOUBLE", RegExp("[0-9]+\\.[0-9]*",NULL),  this, & myScanner::emit);
        define("FLOAT",  RegExp("[0-9]+\\.[0-9]*f",NULL), this, & myScanner::emit);
        define("WORD",   RegExp("[:word:]+",NULL),        this, & myScanner::emit );
        define("BLANKS", RegExp("[:blank:]+",NULL),       this, & myScanner::emit);
        define("PUNCT",  RegExp("[,?!]+",NULL),       this, & myScanner::emit);

    }

    virtual ~myScanner() throw()
    {
    }

    inline bool emit(const Token&) throw()
    {
        return true;
    }

    inline bool onENDL(const Token &) throw()
    {
        if(module) module->newLine();
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

    Lexical::Unit::RemoveFrom(lexemes,"BLANKS");
    Lexical::Unit::RemoveFrom(lexemes,"ENDL");


    for(const Lexical::Unit *u = lexemes.head; u; u=u->next )
    {
        std::cerr << u->label;
        for(size_t i=u->label.length();i<=scanner.max_label_length;++i)
        {
            std::cerr << ' ';
        }
        std::cerr << ": ";
        std::cerr << "'" << *u  << "'";
        std::cerr << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()
