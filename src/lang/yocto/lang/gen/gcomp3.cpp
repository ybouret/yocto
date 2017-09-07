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
                const Node       *child    = children.head; assert(child); assert("ID"==child->origin.label);
                const string      label    = child->toString();
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
                for(child=child->next;child;child=child->next)
                {
                    walkRule(topRule,child);
                }
            }

            void gCompiler:: walkRule(Aggregate &parent, const Node *node )
            {
                static const char fn[] = "gCompiler.walkRule: ";
                const string label = node->origin.label;
                std::cerr << "|+" << label << std::endl;
                switch(walkHash(label))
                {
                    case 0: assert("ID"==label);
                        break;

                    case 1: assert("RX"==label);
                    {
                        const string id = node->toString();
                        addTermTo(parent,id);
                    } break;

                    case 2: assert("RS"==label);
                    {
                        const string id = node->toString();
                        addTermTo(parent,id);
                    } break;

                    case 3: assert("SUB"==label);
                        break;

                    case 4: assert("ALT"==label);
                        break;

                    case 5: assert("OPT"==label);
                        break;

                    case 6: assert("OOM"==label);
                        break;

                    case 7: assert("ZOM"==label);
                        break;

                    default:
                        throw exception("%sunknowm '%s'",fn,label.c_str());
                }
            }


            void gCompiler:: addTermTo(Aggregate &parent, const string &name)
            {
                gTerm *ppT = termDB.search(name);
                if(!ppT)
                {
                    throw exception("gCompiler.addTermTo('%s'): no '%s'", parent.id(), name.c_str());
                }
                parent << **ppT;
            }

#if 0
            Rule & gCompiler:: getSubFrom(const string &label, const Node *node)
            {

            }
#endif
            
        }

    }
}
