#include "yocto/lang/gen/gcompiler.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            const char gCompiler::name[] = "Grammar Compiler";

            gCompiler:: ~gCompiler() throw()
            {
            }

            gCompiler:: gCompiler() :
            getAST(),
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
                    if(node->origin.label=="RULE")
                    {
                        prepareTopLevel(node);
                    }
                }


                return p.yield();
            }


            void gCompiler:: prepareTopLevel(const Node *node)
            {
                assert(node);
                assert("RULE"==node->origin.label);

                const Node::List &children = node->toList();  assert(children.size>0);
                const Node       *child    = children.head;   assert(child->terminal); assert("ID"==child->origin.label);
                const Lexeme     &lex      = child->toLex();
                const string      label    = lex.toString();
                std::cerr << "New TopLevel " << label << std::endl;

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

