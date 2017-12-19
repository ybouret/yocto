#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/exception.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ios/icstream.hpp"

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
                parser.release();
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
                checkIncludes(master);

#if 0
                // debug: checkIncludes
                if(verbose)
                {
                    std::cerr << "== Dynamo Input2" << std::endl;
                    master->graphviz("dyninput2.dot");
                    ios::graphviz_render("dyninput2.dot");
                }
#endif
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

            void DynamoCompiler::serialize( Node *master, ios::ostream &fp )
            {
                if(!master) throw exception("DynamoCompiler.serialize(NULL)");
                parser.release();
                auto_ptr<Node> guard(master);
                checkIncludes(master);
                master->serialize(fp);
            }

            static inline
            Parser * __GenerateFrom(Module    *module ,
                                    const bool verbose)
            {
                Source                   source(module);
                auto_ptr<DynamoCompiler> compiler( new DynamoCompiler() );

                compiler->verbose = verbose;

                Node *master = compiler->parse(source);
                return compiler->encode(master);
            }

            Parser * Parser::GenerateFromFile(const string &filename,
                                              const bool    verbose)
            {
                return __GenerateFrom( Module::OpenFile(filename), verbose );
            }

            Parser * Parser::GenerateFromData(const char    *buffID,
                                              const void    *buffer,
                                              const size_t   buflen,
                                              const bool     verbose)
            {
                return __GenerateFrom(  Module::OpenData(buffID,buffer,buflen), verbose );
            }

            void Parser:: Compile(Module       *grammarInput,
                                  ios::ostream &syntaxOutput,
                                  const bool    verbose)
            {
                // prepare the compiler
                Source                   source(grammarInput);
                auto_ptr<DynamoCompiler> compiler( new DynamoCompiler() );
                compiler->verbose = verbose;

                // parse the source into the grammar syntax tree
                Node *master = compiler->parse(source);

                // serialize the master node
                compiler->serialize(master,syntaxOutput);
            }

        }
    }
}
