#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/lang/syntax/rules.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"

using namespace yocto;
using namespace Lang;


class myLexer : public Lexical::Translator
{
public:
    explicit myLexer() : Lexical::Translator("myLexer")
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
        Syntax::Aggregate &CODE = add( new Syntax::Aggregate("CODE") );
        CODE << ZeroOrMore(ALT1);
        setTopLevel( CODE );
        std::cerr << "saving grammar..." << std::endl;
        graphviz("gram.dot");
        ios::graphviz_render("gram.dot");

        compile();
    }

    virtual ~myGram() throw()
    {
    }

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(myGram);
};

YOCTO_UNIT_TEST_IMPL(gram)
{
    vfs           &fs = local_fs::instance();

    fs.try_remove_file("tree.dot");
    fs.try_remove_file("tree.png");
    fs.try_remove_file("gram.dot");
    fs.try_remove_file("gram.png");

    Module::Handle hm( new Module() );
    Source         source( hm );
    myLexer        lexer;
    myGram         G;

    std::cerr << "Ready..." << std::endl;
    auto_ptr<Syntax::Node> tree( G.accept(lexer,source) );
    if(tree.is_valid())
    {
        tree->graphviz("tree.dot");
        ios::graphviz_render("tree.dot");
    }
    else
    {
        std::cerr << "Empty Tree!" << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()
