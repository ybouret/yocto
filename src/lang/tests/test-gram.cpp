#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/lang/syntax/terminal.hpp"

#include "yocto/utest/run.hpp"


using namespace yocto;
using namespace Lang;


class myLexer : public Lexical::Translator
{
public:
    explicit myLexer() : Lexical::Translator("myLexer","root")
    {

        root.make("INT",   "[0-9]+",    YOCTO_LANG_LEXICAL(forward));
        root.make("WORD",  "[:word:]+", YOCTO_LANG_LEXICAL(forward));
        root.make("ENDL",  "[:endl:]",  YOCTO_LANG_LEXICAL(newline));
        root.make("BLANKS","[:blank:]+",YOCTO_LANG_LEXICAL(discard));

    }

    virtual ~myLexer() throw()
    {
    }

    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(myLexer);
};

class myGram : public Syntax::Grammar
{
public:
    explicit myGram() : Syntax::Grammar("myGrammar")
    {
        Syntax::Rule &INT = append( new Syntax::Terminal("INT" ) );

    }

    virtual ~myGram() throw()
    {
    }

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(myGram);
};

YOCTO_UNIT_TEST_IMPL(gram)
{
    Module::Handle hm( new Module() );
    Source         source( hm );
    myLexer        lexer;
    myGram         G;

    auto_ptr<Syntax::Node> tree( G.accept(lexer,source) );

}
YOCTO_UNIT_TEST_DONE()
