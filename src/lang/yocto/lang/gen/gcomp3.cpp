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
                    topRule << walkRule(child);
                }
            }

            Rule & gCompiler:: walkRule(const Node *node )
            {
                static const char fn[] = "gCompiler.walkRule: ";
                const string label = node->origin.label;
                std::cerr << "|+" << label << std::endl;
                switch(walkHash(label))
                {
                    case 0: assert("ID"==label);
                    {
                        const  string id = node->toString();
                        return getRule(id);
                    }
                        
                    case 1: assert("RX"==label);
                    {
                        const  string id = node->toString();
                        return getTerm(id);
                    }

                    case 2: assert("RS"==label);
                    {
                        const string id = node->toString();
                        return getTerm(id);
                    }
                    case 3: assert("SUB"==label);
                        break;

                    case 4: assert("ALT"==label);
                        break;

                    case 5: assert("OPT"==label);
                        return parser->optional(walkRule(node->head()));
                        break;

                    case 6: assert("OOM"==label);
                        return parser->oneOrMore(walkRule(node->head()));

                    case 7: assert("ZOM"==label);
                        return parser->zeroOrMore(walkRule(node->head()));

                    default:
                        break;

                }
                throw exception("%sunhandled '%s'",fn,label.c_str());
            }


            Rule  &gCompiler:: getTerm(const string &name)
            {
                gTerm *ppT = termDB.search(name);
                if(!ppT)
                {
                    throw exception("gCompiler.getTerm: no '%s'", name.c_str());
                }
                return **ppT;
            }

            Rule  &gCompiler:: getRule(const string &name)
            {
                gRule *ppR = ruleDB.search(name);
                if(ppR)
                {
                    return **ppR;
                }
                else
                {
                    gTerm *ppT = termDB.search(name);
                    if(!ppT)
                    {
                        throw exception("gCompiler.getRule: no '%s'", name.c_str());
                    }
                    return **ppT;
                }
            }

            
#if 0
            Rule & gCompiler:: getSubFrom(const string &label, const Node *node)
            {
                
            }
#endif
            
        }
        
    }
}
