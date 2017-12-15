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
                if(!master) throw exception("DynamoCompiler.encode(NULL)");
                auto_ptr<Node> guard(master);
#if 1
                if(verbose)
                {
                    std::cerr << "== Dynamo Input" << std::endl;
                    master->graphviz("dyninput.dot");
                    ios::graphviz_render("dyninput.dot");
                }
#endif
                parser.release();
                checkIncludes(master);
                if(true)
                {
                    const string sh1 = master->signature();
                    std::cerr << "signature1=" << sh1 << std::endl;
                    {
                        std::cerr << "serialize master node" << std::endl;
                        ios::wcstream fp("master.dat");
                        master->serialize(fp);
                    }

                    {
                        std::cerr << "reload master node" << std::endl;
                        Source S( Module::OpenFile("master.dat") );
                        auto_ptr<Node> reloaded( Node::loadFrom(S,*this) );
                        const string sh2 = reloaded->signature();
                        std::cerr << "signature2=" << sh2 << std::endl;
                    }
                }


#if 1
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



        }
    }
}
