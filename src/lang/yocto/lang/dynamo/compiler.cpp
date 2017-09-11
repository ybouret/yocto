#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/ios/graphviz.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            DynamoCompiler:: ~DynamoCompiler() throw()
            {
            }

            static const char *topKW[] =
            {
                "RULE",
                "ALIAS",
                "PLUG",
                "LEXR",
                "SEMR"
            };

            static const char *strKW[] =
            {
                "RX",
                "RS"
            };

            static const char *lnkKW[] =
            {
                "ID",
                "RX",
                "RS",
                "SUB",
                "ALT",
                "ZOM",
                "OOM",
                "OPT"
            };

            DynamoCompiler:: DynamoCompiler() :
            DynamoParser(),
            parser(0),
            verbose(false),
            ruleDB(),
            termDB(),
            topHash(YOCTO_MPERF_FOR(topKW)),
            strHash(YOCTO_MPERF_FOR(strKW)),
            lnkHash(YOCTO_MPERF_FOR(lnkKW))
            {
            }




            Parser *DynamoCompiler:: encode( Node *master )
            {
                auto_ptr<Node> guard(master);
                parser.reset(0);
                createTopLevelFrom(master);
                gatherFrom(master);

                if(verbose)
                {
                    std::cerr << "== rendering " << parser->tag << " ==" << std::endl;
                    const string parserDot = parser->tag + ".dot";
                    parser->graphviz(parserDot);
                    ios::graphviz_render(parserDot);
                }

                return parser.yield();
            }
            
        }
    }
}
