#include "yocto/lang/syntax/parser.hpp"
#include "yocto/lang/lexical/plugin/cstring.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"

using namespace yocto;
using namespace Lang;

namespace
{
    static const char rx_num[] =
    "[-+]?[0-9]+";

    class jParser : public Syntax::Parser
    {
    public:
        explicit jParser() : Syntax::Parser("JSON")
        {
            Syntax::Rule &jTrue   = terminal("true").let(IsUnique);
            Syntax::Rule &jFalse  = terminal("false").let(IsUnique);
            Syntax::Rule &jNull   = terminal("null").let(IsUnique);
            Syntax::Rule &jNumber = terminal("number",rx_num);
            Syntax::Rule &jString = term<Lexical::cstring>("string");

            Syntax::Alternate &jValue = alt();
            jValue << jNumber << jTrue << jFalse << jNull << jString;

            setTopLevel(ZeroOrMore(jValue));

            root.make("ENDL",  "[:endl:]",   YOCTO_LANG_LEXICAL(newline));
            root.make("BLANKS","[:blank:]+", YOCTO_LANG_LEXICAL(discard));

            compile();

            graphviz("json.dot");
            ios::graphviz_render("json.dot");

        }

        virtual ~jParser() throw()
        {
        }


    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(jParser);
    };
}

YOCTO_UNIT_TEST_IMPL(json)
{
    vfs &fs = local_fs::instance();
    fs.try_remove_file("json.dot");
    fs.try_remove_file("json.png");
    fs.try_remove_file("tree.dot");
    fs.try_remove_file("tree.png");

    jParser        J;
    {
        Module::Handle hm( new Module() );
        Source         source( hm );
        auto_ptr<Syntax::Node> tree( J(source) );
        if(tree.is_valid())
        {
            tree->graphviz("tree.dot");
            ios::graphviz_render("tree.dot");
        }
    }

}
YOCTO_UNIT_TEST_DONE()
