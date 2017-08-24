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
        root.make("blanks","[:blank:]+", this, &myTrans::emit);
        root.make("endl",  "[:endl:]+",  this, &myTrans::endl);

        Lexical::Scanner &com1 = declare("com1");
    }

    virtual ~myTrans() throw()
    {
    }

    Lexical::Result emit(const Token &) throw()
    {
        return Lexical::Forward;
    }

    Lexical::Result endl(const Token &) throw()
    {
        root.module->newLine();
        return Lexical::Forward;
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
