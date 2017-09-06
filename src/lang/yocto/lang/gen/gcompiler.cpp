#include "yocto/lang/gen/gcompiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/lang/lexical/plugin/cstring.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"

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

            static const char *RuleKeywords[] =
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
            ruleHash(YOCTO_MPERF_FOR(RuleKeywords)),
            lexrHash(YOCTO_MPERF_FOR(LexrKeywords)),
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
                // Declare the top-level rules and plugins
                //______________________________________________________________
                registerTopLevelRules(topNode);
                {
                    parser->graphviz("parser.dot");
                    ios::graphviz_render("parser.dot");
                }

                return parser.yield();
            }


            void gCompiler:: registerTopLevelRules(const Node *node)
            {
                for(;node;node=node->next)
                {
                    const string &label = node->origin.label;
                    switch(rootHash(label))
                    {
                        case 0: assert( "RULE" == label );
                            registerNewRule(node);
                            break;

                        case 1: assert( "LXR" == label );
                            detectPlugin(node);
                            break;

                        default:
                            break;
                    }
                }
            }

            void gCompiler::registerNewRule(const Node *node)
            {
                const Node::List &children = node->toList();  assert(children.size>0);
                const Node       *child    = children.head;   assert(child->terminal); assert("ID"==child->origin.label);
                const string      label    = child->toString();
                std::cerr << "New RULE " << label << std::endl;

                Aggregate &newRule = parser->agg(label);
                if( !ruleDB.insert(label,&newRule) )
                {
                    throw exception("registerNewRule: unexpected failure for '%s'", label.c_str());
                }
            }

            void gCompiler::detectPlugin(const Node *lxr)
            {
                static const char fn[] = "gCompiler.detectPlugin: ";

                //______________________________________________________________
                //
                // get the info
                //______________________________________________________________

                const Node::List   &code = lxr->toList(); assert(code.size>0);
                const Node         *node = code.head;     assert(node!=NULL); assert("LX"==node->origin.label);
                const string        name = node->toString(1);
                std::cerr << "new LXR " << name << std::endl;

                //______________________________________________________________
                //
                // check for reserved word
                //______________________________________________________________
                if( lexrHash(name) >= 0) return; //! a reserved word

                //______________________________________________________________
                //
                // check syntax
                //______________________________________________________________
                std::cerr << "\twill use a plugin for '" << name << "'" << std::endl;
                if(code.size!=2)
                {
                    throw exception("%s@%s must have exactly ONE plugin name",fn,name.c_str());
                }

                //______________________________________________________________
                //
                // get the plugin name
                //______________________________________________________________
                node=node->next;
                assert("RX"==node->origin.label||"RS"==node->origin.label);
                const string pluginName = node->toString();
                std::cerr << "\t\tpluginName=" << pluginName << std::endl;

                //______________________________________________________________
                //
                // hardcoded plugins
                //______________________________________________________________
                Terminal *t = 0;
                if( "cstring" == pluginName )
                {
                    t = & parser->term<Lexical::cstring>(name);
                    goto REGISTER_TERM;
                }

                if( "rstring" == pluginName )
                {
                    t = & parser->term<Lexical::rstring>(name);
                    goto REGISTER_TERM;
                }

                throw exception("%splugin '%s' is not implemented", fn, pluginName.c_str() );
            REGISTER_TERM:
                assert(NULL!=t);
                if(!termDB.insert(name,t))
                {
                    throw exception("%sunexpected failure for '%s'",fn,name.c_str());
                }
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
        }

    }
}

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            gNode:: ~gNode() throw()
            {
            }

            gNode:: gNode(const string  &id,
                          const uint32_t t) :
            next(0),
            prev(0),
            label(id),
            uuid(t)
            {
            }
            
        }
    }
}

