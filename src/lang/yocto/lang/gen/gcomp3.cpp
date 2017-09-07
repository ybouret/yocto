#include "yocto/lang/gen/gcompiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {


            void gCompiler::link(const Node *topNode)
            {
                if(verbose)
                {
                    std::cerr << "== linking ==" << std::endl;
                }

                for(const Node *node = topNode;node;node=node->next)
                {
                    const string &label = node->origin.label;
                    switch(rootHash(label))
                    {
                        case 0: assert( "RULE" == label );
                            linkRule(node);
                            break;

                        case 1: assert( "LXR" == label );
                            break;

                        case 2: assert( "SMR" == label );
                            break;
                            
                        default:
                            throw exception("gCompiler.link: unknowm \"%s\"", label.c_str());
                    }
                }
            }


            void gCompiler:: linkRule(const Node *node)
            {
                const Node::List &children = node->toList();
                const string      label    = children.head->toString();
                std::cerr << "linking '" << label << "'" << std::endl;

                gRule *ppR = ruleDB.search(label);
                if(!ppR)
                {
                    gTerm *ppT = termDB.search(label);
                    if(!ppT)
                    {
                        throw exception("gCompiler.linkRule: unregistered '%s'", label.c_str());
                    }
                    return;
                }

                Aggregate &topRule = **ppR;
                for(node=node->next;node;node=node->next)
                {
                    walkRule(topRule,node);
                }
            }

            void gCompiler:: walkRule(Aggregate &parent, const Node *node )
            {

            }

        }

    }
}
