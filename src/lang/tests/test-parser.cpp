#include "yocto/lang/syntax/parser.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"

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
        Syntax::Rule      &END  = terminal("END", ";");
        Syntax::Rule      &ID   = terminal("ID", "[:word:]+");
        Syntax::Aggregate &DECL = agg("DECL");
        DECL << ID << END;

        setTopLevel(ZeroOrMore(DECL));

        hook<Lexical::Comment>("com1","//");
        root.call("com1");

        graphviz("gram.dot");
        ios::graphviz_render("gram.dot");

        root.make("ENDL",  "[:endl:]",   YOCTO_LANG_LEXICAL(newline));
        root.make("BLANKS","[:blank:]+", YOCTO_LANG_LEXICAL(discard));

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

}
YOCTO_UNIT_TEST_DONE()
