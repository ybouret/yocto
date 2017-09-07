#include "yocto/lang/gen/gcompiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/fs/local-fs.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {


            gCompiler:: ~gCompiler() throw()
            {
            }

            static const char *RootKeywords[] =
            {
                "RULE",
                "LXR",
                "SMR"
            };

            static const char *TermKeywords[] =
            {
                "ID",
                "RX",
                "RS"
            };



            static const char *LexrKeywords[] =
            {
                "drop",
                "endl",
                "comment"
            };


            static const char *WalkKeywords[] =
            {
                "ID",  // 0
                "RX",  // 1
                "RS",  // 2
                "SUB", // 3
                "ALT", // 4
                "OPT", // 5
                "OOM", // 6
                "ZOM"  // 7
            };

            gCompiler:: gCompiler() :
            getAST(),
            parser(NULL),
            rootHash(YOCTO_MPERF_FOR(RootKeywords)),
            lexrHash(YOCTO_MPERF_FOR(LexrKeywords)),
            termHash(YOCTO_MPERF_FOR(TermKeywords)),
            walkHash(YOCTO_MPERF_FOR(WalkKeywords)),
            ruleDB(),
            termDB(),
            verbose(false)
            {

            }
            
        }
    }
}

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {


            Parser * gCompiler:: encode(const Node *ast)
            {
                assert(ast);
                assert(ast->internal);
                assert(ast->toList().size>=1);

                //______________________________________________________________
                //
                // cleanup
                //______________________________________________________________
                termDB.free();
                ruleDB.free();
                parser.release();

                //______________________________________________________________
                //
                // starting from toplevel
                //______________________________________________________________
                const Node::List &topList    = ast->toList();
                const Node       *topNode    = topList.head;

                //______________________________________________________________
                //
                // getting the name, first thing first !
                //______________________________________________________________
                const string      parserName = topNode->toLex().toString(1);
                if(verbose) { std::cerr << "Creating parser '" << parserName << "'" << std::endl; }
                parser.reset( new Parser(parserName) );
                topNode = topNode->next;

                if(verbose)
                {
                    static vfs &fs = local_fs::instance();
                    const string parserNameAST    = parserName+"_ast.dot";
                    const string parserNameASTgfx = parserName+"_ast.png";
                    fs.try_remove_file(parserNameAST);
                    fs.try_remove_file(parserNameASTgfx);

                    ast->graphviz(parserNameAST);
                    ios::graphviz_render(parserNameAST);
                }

                //______________________________________________________________
                //
                // Pass 1: Declare the top-level rules and plugins
                //______________________________________________________________
                registerTopLevelRules(topNode);


                //______________________________________________________________
                //
                // Pass 2: check rules's ID and create terminals
                //______________________________________________________________
                registerTermsAndCheckRules(topNode);



                //______________________________________________________________
                //
                // Pass 3: walk the tree and create stuff
                //______________________________________________________________
                link(topNode);

                if(verbose)
                {
                    static vfs  &fs = local_fs::instance();
                    const string parserOutDot = parserName + ".dot";
                    const string parserOutGfx = parserName + ".png";
                    fs.try_remove_file(parserOutDot);
                    fs.try_remove_file(parserOutGfx);
                    parser->graphviz(parserOutDot);
                    ios::graphviz_render(parserOutDot);
                }
                
                termDB.free();
                ruleDB.free();
                return parser.yield();
            }


            
            Rule & gCompiler:: find(const string &id)
            {
                {
                    gRule *ppR = ruleDB.search(id);
                    if(ppR) return **ppR;
                }

                {
                    gTerm *ppT = termDB.search(id);
                    if(ppT) return **ppT;
                }

                throw exception("gCompiler: no registered '%s'", id.c_str());
            }

            bool gCompiler:: has(const string &id) const throw()
            {
                return (NULL!=ruleDB.search(id)) || (NULL!=termDB.search(id));
            }
        }

    }
}

