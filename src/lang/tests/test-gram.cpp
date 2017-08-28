#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/lang/syntax/rules.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"

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
        Syntax::Rule      &INT  = add( new Syntax::Terminal("INT" )   );
        Syntax::Rule      &WORD = add( new Syntax::Terminal("WORD")   );
        Syntax::Alternate &ALT1 = add( new Syntax::Alternate("ALT#1") );
        ALT1 << INT << WORD;

        setTopLevel(ALT1);

        std::cerr << "saving grammar..." << std::endl;
        graphviz("gram.dot");
        ios::graphviz_render("gram.dot");
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
