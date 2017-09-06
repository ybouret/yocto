#include "yocto/lang/gen/gcompiler.hpp"
#include "yocto/exception.hpp"

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

            gCompiler:: gCompiler() :
            getAST(),
            RootHash(YOCTO_MPERF_FOR(RootKeywords)),
            RuleHash(YOCTO_MPERF_FOR(RuleKeywords)),
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
                auto_ptr<Parser>  p( new Parser(parserName) );
                topNode = topNode->next;

                //______________________________________________________________
                //
                // now gather information by walking the tree
                //______________________________________________________________

                //______________________________________________________________
                //
                // first pass: top level rules and plugins
                //______________________________________________________________
                for(const Node *node = topNode; node; node=node->next )
                {
                    const string &label = node->origin.label;
                    switch(RootHash(label))
                    {
                        case 0: assert( "RULE" == label );
                            break;

                        case 1: assert( "LXR" == label );
                            break;

                        case 2: assert( "SMR" == label);
                            break;

                        default:
                            throw exception("Unknowm label '%s'", label.c_str());
                    }
                }


                return p.yield();
            }

#if 0
            void gCompiler:: onRULE(const Node *node)
            {
                assert(node);
                assert("RULE"==node->origin.label);

                const Node::List &children = node->toList();  assert(children.size>0);
                const Node       *child    = children.head;   assert(child->terminal); assert("ID"==child->origin.label);
                const Lexeme     &lex      = child->toLex();
                const string      label    = lex.toString();
                std::cerr << "New RULE " << label << std::endl;
                collect(child->next);
            }

            void gCompiler:: collect(const Node *node)
            {
                if(!node) return;
                
                if(node->terminal)
                {
                    const string &label   = node->origin.label;
                    const string  content = node->toLex().toString();
                    switch( RuleHash(label) )
                    {
                        case 0: assert( "ID" == label );
                            std::cerr << "\tnew AGG " << content << std::endl;
                            return;

                        case 1: assert( "RX" == label );
                            std::cerr << "\tnew RX " << content << std::endl;
                            return;

                        case 2: assert( "RS" == label );
                            std::cerr << "\tnew RS " << content << std::endl;
                            return;

                        case 3: assert( "RB" == label);
                            std::cerr << "\tnew RB " << content << std::endl;
                            return;

                        default:
                            throw exception("Unknown label '%s'", label.c_str());
                    }

                }
                else
                {
                    const Node::List &children = node->toList();
                    for(const Node *child=children.head;child;child=child->next)
                    {
                        collect(child);
                    }
                }
            }

            void gCompiler:: onLXR(const Node *node)
            {
                assert("LXR"==node->origin.label);
                assert(node->internal);
                const Node::List &lxr = node->toList(); assert(lxr.size>0);
                const Node       *sub = lxr.head;       assert(sub!=NULL); assert( "LX" == sub->origin.label );


            }
#endif

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

