#include "yocto/lang/syntax/parser.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"

#include "yocto/utest/run.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"

using namespace yocto;
using namespace Lang;

namespace
{
    static const char rx_num[] =
    "[0-9]+";

    class jParser : public Syntax::Parser
    {
    public:
        explicit jParser() : Syntax::Parser("JSON")
        {
            Syntax::Rule &jTrue   = terminal("true").let(IsUnique);
            Syntax::Rule &jFalse  = terminal("false").let(IsUnique);
            Syntax::Rule &jNull   = terminal("null").let(IsUnique);
            Syntax::Rule &jNumber = terminal("number",rx_num);

            Syntax::Aggregate &jValue = agg("value");
            jValue << jNumber << jTrue << jFalse << jNull;

            setTopLevel(ZeroOrMore(jValue));
            compile();
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

    jParser J;

}
YOCTO_UNIT_TEST_DONE()
