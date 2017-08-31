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
            Syntax::Rule     &jTrue   = terminal("true").let(IsUnique);
            Syntax::Rule     &jFalse  = terminal("false").let(IsUnique);
            Syntax::Rule     &jNull   = terminal("null").let(IsUnique);
            Syntax::Rule     &jNumber = terminal("number",rx_num);
            Syntax::Rule     &jString = term<Lexical::cstring>("string");
            Syntax::Rule     &jComma  = terminal(',').let(IsHollow);
            Syntax::Rule     &jLBRACK = terminal('[').let(IsHollow);
            Syntax::Rule     &jRBRACK = terminal(']').let(IsHollow);

            Syntax::Compound &jElements   = agg("elements");
            Syntax::Compound &jElementsEx = agg("elements#ex");

            Syntax::Alternate &jValue = alt();
            jValue << jNumber << jTrue << jFalse << jNull << jString;

            jElements << jValue << ZeroOrMore( jElementsEx << jComma << jValue );

            Syntax::Aggregate &jArray = agg("array");
            jArray << jLBRACK << jElements << jRBRACK;

            Syntax::Aggregate &jEmptyArray = agg("empty_array");
            jEmptyArray << jLBRACK << jRBRACK;

            Syntax::Rule &jArrays = Choice(jArray,jEmptyArray);

            jValue << jArray << jEmptyArray;

            setTopLevel(jArrays);


            // final lexical rules
            root.make("ENDL",  "[:endl:]",   YOCTO_LANG_LEXICAL(newline));
            root.make("BLANKS","[:blank:]+", YOCTO_LANG_LEXICAL(discard));

            std::cerr << "Compiling..." << std::endl;
            compile();

            std::cerr << "Rendering..." << std::endl;
            graphviz("json.dot");
            ios::graphviz_render("json.dot");
            std::cerr << "...done" << std::endl;

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
        std::cerr << std::endl << "Ready..." << std::endl;
        auto_ptr<Syntax::Node> tree( J(source) );
        if(tree.is_valid())
        {
            tree->graphviz("tree.dot");
            ios::graphviz_render("tree.dot");
        }
    }

}
YOCTO_UNIT_TEST_DONE()
