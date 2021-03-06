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
    Lexical::Translator("lexer"),
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
        com2.make("endl","[:endl:]", this, &myTrans::com2ENDL);
        com2.make("any1", ".",       this, &myTrans::com2ANY1);
    }

    virtual ~myTrans() throw()
    {
        //std::cerr << "~myTrans" << std::endl;
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
        newLine();
        return Lexical::Discard;
    }

    void enterCom1(const Token &)
    {
    }

    void leaveCom1(const Token &)
    {
        newLine();
    }

    void enterCom2(const Token &)
    {
        data.clear();
    }

    void leaveCom2(const Token&)
    {
        std::cerr << "COM2: " << data << std::endl;
        unget( com2.newUnit(data) );
    }

    Lexical::Result com2ENDL(const Token &tkn)
    {
        newLine();
        data.add(tkn);
        return Lexical::Discard;
    }

    Lexical::Result com2ANY1(const Token &tkn)
    {
        data.add(tkn);
        return Lexical::Discard;
    }

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(myTrans);
};


YOCTO_UNIT_TEST_IMPL(trans)
{
    myTrans        trans;
    Source         source( Module::OpenSTDIN() );       // register it in source

    Lexical::Units lexemes;

    if(argc>1)
    {
        const string arg = argv[1];
        if(arg=="prefetch")
        {
            Lexical::Units tmp;
            trans.getAll(tmp,source);
            trans.ungetCopyOf(tmp);
        }
    }

    for(;;)
    {
        Lexical::Unit *u = trans.get(source);
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

        
