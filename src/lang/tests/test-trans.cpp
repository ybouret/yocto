#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/lang/lexical/translator.hpp"
#include "yocto/utest/run.hpp"


using namespace yocto;
using namespace Lang;

class myTrans : public Lexical::Translator
{
public:

    explicit myTrans() : Lexical::Translator("lexer","main")
    {

        root.make("int",   "[0-9]+",     this, &myTrans::emit);
        root.make("ID",    "[:word:]+",  this, &myTrans::emit);
        root.call("com1","//",this, &myTrans::enterCom1);
        root.make("blanks","[:blank:]+", this, &myTrans::drop);
        root.make("endl",  "[:endl:]",   this, &myTrans::endl);

        Lexical::Scanner &com1 = declare("com1");
        com1.back("[:endl:]", this, &myTrans::leaveCom1);
        com1.make("other",".",this, &myTrans::drop);

    }

    virtual ~myTrans() throw()
    {
    }

    Lexical::Result emit(const Token &tkn) throw()
    {
        std::cerr << "EMIT '" << tkn << "'" << std::endl;
        return Lexical::Forward;
    }

    Lexical::Result drop(const Token &tkn) throw()
    {
        std::cerr << "DROP '" << tkn << "'" << std::endl;
        return Lexical::Discard;
    }

    Lexical::Result endl(const Token &) throw()
    {
        std::cerr << "NEWLINE" << std::endl;
        root.module->newLine();
        return Lexical::Discard;
    }

    void enterCom1(const Token &)
    {
        std::cerr << "<COM1>" << std::endl;
    }

    void leaveCom1(const Token &)
    {
        std::cerr << "</COM1>" << std::endl;
    }



private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(myTrans);
};


YOCTO_UNIT_TEST_IMPL(trans)
{
    myTrans trans;
    Module::Handle hModule( new Module() ); // open stdio
    Source         source( hModule );       // register it in source


    Lexical::Units lexemes;

    for(;;)
    {
        Lexical::Result res  = Lexical::Forward;
        Lexical::Unit  *lex  = trans.root.probe(source,res);
        if(!lex) break;
        lexemes.push_back(lex);
    }

    for(const Lexical::Unit *u = lexemes.head; u; u=u->next )
    {
        std::cerr << u->label;
        for(size_t i=u->label.length();i<=trans.root.max_label_length;++i)
        {
            std::cerr << ' ';
        }
        std::cerr << ": ";
        std::cerr << "'" << *u  << "'";
        std::cerr << std::endl;
    }
    
    
}
YOCTO_UNIT_TEST_DONE()
