#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/lang/lexical/translator.hpp"
#include "yocto/utest/run.hpp"


using namespace yocto;
using namespace Lang;

class myTrans : public Lexical::Translator
{
public:

    Lexical::Scanner &com1;
    Lexical::Scanner &com2;
    Token             data;

    explicit myTrans() :
    Lexical::Translator("lexer","main"),
    com1( declare("com1") ),
    com2( declare("com2") ),
    data()
    {

        root.make("int",   "[0-9]+",     this, &myTrans::emit);
        root.make("ID",    "[:word:]+",  this, &myTrans::emit);
        root.call("com1","//",this, &myTrans::enterCom1);
        root.make("blanks","[:blank:]+", this, &myTrans::drop);
        root.make("endl",  "[:endl:]",   this, &myTrans::endl);

        com1.back("[:endl:]",  this, &myTrans::leaveCom1);
        com1.make("other",".", this, &myTrans::dropCom);

        root.call("com2","/\\*",this, &myTrans::enterCom2);

        com2.back("\\*/",this,&myTrans::leaveCom2);

    }

    virtual ~myTrans() throw()
    {
    }

    Lexical::Result emit(const Token &tkn) throw()
    {
        return Lexical::Forward;
    }

    Lexical::Result drop(const Token &tkn) throw()
    {
        return Lexical::Discard;
    }

    Lexical::Result dropCom(const Token &tkn) throw()
    {
        return Lexical::Discard;
    }

    Lexical::Result endl(const Token &) throw()
    {
        root.module->newLine();
        return Lexical::Discard;
    }

    void enterCom1(const Token &)
    {
    }

    void leaveCom1(const Token &)
    {
        com1.module->newLine();
    }

    void enterCom2(const Token &)
    {
        data.clear();
    }

    void leaveCom2(const Token&)
    {

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
        Lexical::Unit *u = trans.run(source);
        if(u) lexemes.push_back(u); else break;
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
