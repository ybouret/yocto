#include "yocto/lang/syntax/parser.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"
#include "yocto/lang/lexical/plugin/cstring.hpp"
#include "yocto/lang/lexical/plugin/string64.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"

using namespace yocto;
using namespace Lang;

class myParser : public Syntax::Parser
{
public:
    explicit myParser() : Syntax::Parser("dummy")
    {
        Syntax::Aggregate &ASSIGN = agg("ASSIGN");
        Syntax::Rule      &END    = terminal("END", ";").let(IsHollow);
        Syntax::Rule      &ID     = terminal("ID", "[_[:alpha:]][:word:]*");
        Syntax::Rule      &INT    = terminal("INT","[:digit:]+");

        ASSIGN << ID << terminal("EQUAL",'=').let(IsHollow) << choice(ID,INT) << END;
        Syntax::Aggregate &DECL = agg("DECL");
        DECL << choice(ID,INT) << END;


        setTopLevel(zeroOrMore(choice(ASSIGN,DECL)));

        hook<Lexical::Comment>("com1","//");
        root.call("com1");

        hook<Lexical::Comment>("com2","#");
        root.call("com2");

        root.call(hook<Lexical::InlineComment>("com3","/\\*","\\*/"));

        //root.call(hook<Lexical::cstring>("string"));
        //root.call(hook<Lexical::String64>("b64"));


        root.make("ENDL",  "[:endl:]",   YOCTO_LANG_LEXICAL(newline));
        root.make("BLANKS","[:blank:]+", YOCTO_LANG_LEXICAL(discard));

        graphviz("gram.dot");
        ios::graphviz_render("gram.dot");

        check_connectivity();
    }

    virtual ~myParser() throw()
    {
    }

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(myParser);
};

YOCTO_UNIT_TEST_IMPL(parser)
{
    vfs           &fs = local_fs::instance();

    fs.try_remove_file("tree.dot");
    fs.try_remove_file("tree.png");
    fs.try_remove_file("gram.dot");
    fs.try_remove_file("gram.png");

    Module::Handle hm( new Module() );
    Source         source( hm );
    myParser       parser;

    auto_ptr<Syntax::Node> tree( parser.parse(source) );
    if( tree.is_valid() )
    {
        tree->graphviz("tree.dot");
        ios::graphviz_render("tree.dot");
    }
}
YOCTO_UNIT_TEST_DONE()
