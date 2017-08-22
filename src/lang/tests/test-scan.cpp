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
        make("ENDL",   RegExp("[:endl:]",NULL),         this, & myScanner::onENDL );
        make("INT",    RegExp("[0-9]+",NULL),           this, & myScanner::emit);
        make("DOUBLE", RegExp("[0-9]+\\.[0-9]*",NULL),  this, & myScanner::emit);
        make("FLOAT",  RegExp("[0-9]+\\.[0-9]*f",NULL), this, & myScanner::emit);
        make("WORD",   RegExp("[:word:]+",NULL),        this, & myScanner::emit );
        make("BLANKS", RegExp("[:blank:]+",NULL),       this, & myScanner::emit);
        make("PUNCT",  RegExp("[,?!]+",NULL),           this, & myScanner::emit);
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
        assert(module);
        module->newLine();
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
