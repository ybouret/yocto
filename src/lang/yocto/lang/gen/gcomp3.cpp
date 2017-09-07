#include "yocto/lang/gen/gcompiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"
#include "yocto/code/utils.hpp"

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
                            linkLexr(node);
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

            Rule & gCompiler:: walkRule(const Node *node)
            {
                static const char fn[] = "gCompiler.walkRule: ";
                const string label = node->origin.label;
                if(verbose)
                {
                    std::cerr << "|+" << label << std::endl;
                }
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
                    {
                        Aggregate &sub = parser->agg( parser->newAggLabel() );
                        fillCompound(sub,node);
                        return sub;
                    }

                    case 4: assert("ALT"==label);
                    {
                        Alternate &alt = parser->alt();
                        fillCompound(alt,node);
                        return alt;
                    }

                    case 5: assert("OPT"==label);
                        return parser->optional(walkRule(node->head()));

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

            
            void gCompiler:: fillCompound(Compound   &cmp,
                                          const Node *from)
            {
                for(const Node *node = from->head(); node; node=node->next)
                {
                    cmp << walkRule(node);
                }
            }


            void gCompiler:: linkLexr(const Node *node)
            {
                const Node::List &children = node->toList(); assert(children.size>0);
                node                       = children.head;  assert(children.head);
                const string      lx       = node->toString(1);

                if(verbose)
                {
                    std::cerr << "|@" << lx << std::endl;
                }

                const string lxr_label_base = parser->Translator::name + '.' + lx;
                switch(lexrHash(lx))
                {
                    case 0: assert("drop"==lx);
                        for(node=node->next;node;node=node->next)
                        {
                            const string lxr_expr  = StringToExpr(node);
                            const string lxr_label = lxr_label_base + '@' + lxr_expr;
                            if(verbose)
                            {
                                std::cerr << " |_" << lxr_label << " '" << lxr_expr << "'" << std::endl;
                            }
                            parser->root.make(lxr_label,lxr_expr, & *parser, & Lexical::Translator::discard);
                        }
                        break;

                    case 1: assert("endl"==lx);
                        for(node=node->next;node;node=node->next)
                        {
                            const string lxr_expr  = StringToExpr(node);
                            const string lxr_label = lxr_label_base + '@' + lxr_expr;
                            if(verbose)
                            {
                                std::cerr << " |_" << lxr_label << " '" << lxr_expr << "'" << std::endl;
                            }
                            parser->root.make(lxr_label,lxr_expr, & *parser, & Lexical::Translator::newline);
                        }
                        break;

                    case 2: assert("comment"==lx);
                    {
                        switch(children.size)
                        {
                            case 2: // one string
                            {
                                const string com_enter = StringToExpr(node->next);
                                const string com_label = lxr_label_base + '@' + com_enter;
                                if(verbose)
                                {
                                    std::cerr << "|_" << com_label << " '" << com_enter << "'" << std::endl;
                                }
                                parser->root.call( parser->hook<Lexical::Comment>(com_label,com_enter));
                            } break;

                            case 3: // two strings
                            {
                                const string com_enter = StringToExpr(node->next);
                                const string com_leave = StringToExpr(node->next->next);
                                const string com_label = lxr_label_base + '@' + com_enter + '@' + com_leave;
                                if(verbose)
                                {
                                    std::cerr << "|_" << com_label << " '" << com_enter << "' -> '" << com_leave << "'" << std::endl;
                                }
                                parser->root.call( parser->hook<Lexical::InlineComment>(com_label,com_enter,com_leave));
                            } break;

                            default:
                                throw exception("@comment: requires 1 or 2 strings");
                        }
                    }

                        break;
                        
                    default:
                        if(verbose)
                        {
                            std::cerr << "|_already processed" << std::endl;
                        }
                }
            }



            string gCompiler:: StringToExpr(const Node *node)
            {
                if("RX" == node->origin.label)
                {
                    return node->toString();
                }

                if("RS" == node->origin.label)
                {
                    const string data = node->toString();
                    const size_t nch  = data.size();
                    string       expr(nch*4,as_capacity);
                    for(size_t i=0;i<nch;++i)
                    {
                        expr << "\\x";
                        expr << hexa_text_lower[uint8_t(data[i])];
                    }
                    return expr;
                }

                throw exception("gCompiler::StringToExpr: unhandled '%s", node->origin.label.c_str());
            }

        }
        
    }
}
