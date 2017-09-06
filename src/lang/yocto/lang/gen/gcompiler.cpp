#include "yocto/lang/gen/gcompiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/ios/graphviz.hpp"

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
                "RS",
                "RB"
            };



            static const char *LexrKeywords[] =
            {
                "drop",
                "endl",
                "comment"
            };

            gCompiler:: gCompiler() :
            getAST(),
            parser(NULL),
            rootHash(YOCTO_MPERF_FOR(RootKeywords)),
            lexrHash(YOCTO_MPERF_FOR(LexrKeywords)),
            termHash(YOCTO_MPERF_FOR(TermKeywords)),
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

                //______________________________________________________________
                //
                // Pass 1: Declare the top-level rules and plugins
                //______________________________________________________________
                registerTopLevelRules(topNode);
                {
                    parser->graphviz("parser.dot");
                    ios::graphviz_render("parser.dot");
                }

                //______________________________________________________________
                //
                // Pass 2: check rules's ID and create terminals
                //______________________________________________________________
                registerTermsAndCheckRules(topNode);

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

                throw exception("No registered '%s'", id.c_str());
            }

            bool gCompiler:: has(const string &id) const throw()
            {
                return (NULL!=ruleDB.search(id)) || (NULL!=termDB.search(id));
            }
        }

    }
}

