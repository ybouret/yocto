#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/exception.hpp"

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

            static const char *lxrKW[] =
            {
                "drop",
                "endl",
                "comment"
            };

            DynamoCompiler:: DynamoCompiler() :
            DynamoParser(),
            parser(0),
            verbose(false),
            ruleDB(),
            termDB(),
            topHash(YOCTO_MPERF_FOR(topKW)),
            strHash(YOCTO_MPERF_FOR(strKW)),
            lnkHash(YOCTO_MPERF_FOR(lnkKW)),
            lxrHash(YOCTO_MPERF_FOR(lxrKW)),
            top_max_size(0)
            {
            }




            Parser *DynamoCompiler:: encode( Node *master )
            {
                if(!master) throw exception("DynamoCompiler.encode(NULL)");
                auto_ptr<Node> guard(master);
#if 0
                if(verbose)
                {
                    std::cerr << "== Dynamo Input" << std::endl;
                    master->graphviz("dyninput.dot");
                    ios::graphviz_render("dyninput.dot");
                }
#endif
                parser.release();
                createTopLevelFrom(master);
                gatherFrom(master);
                finalize();

                if(verbose)
                {
                    std::cerr << "== rendering " << parser->tag << " ==" << std::endl;
                    const string parserDot = parser->tag + ".dot";
                    parser->graphviz(parserDot);
                    ios::graphviz_render(parserDot);
                    std::cerr << std::endl;
                }

                return parser.yield();
            }

            static inline
            Parser * __GenerateFrom( Module *module , const bool verbose)
            {
                const Module::Handle     hModule(module);
                Source                   source(hModule);
                auto_ptr<DynamoCompiler> compiler( new DynamoCompiler() );

                compiler->verbose = verbose;

                Node *master = compiler->parse(source);
                return compiler->encode(master);
            }

            Parser * Parser::Generate(const string &filename,
                                      const bool    verbose)
            {
                return __GenerateFrom( new Module(filename), verbose );
            }

            Parser * Parser::Generate(const void    *buffer,
                                      const size_t   buflen,
                                      const bool     verbose)
            {
                return __GenerateFrom( new Module(buffer,buflen), verbose );
            }


           
        }
    }
}
