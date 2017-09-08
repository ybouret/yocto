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


            //__________________________________________________________________
            //
            //
            // Top Level Linking call
            //
            //__________________________________________________________________
            void gCompiler::link(const Node *topNode)
            {
                if(verbose)
                {
                    std::cerr << "== linking {" << parser->Grammar::name << "} ==" << std::endl;
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
                            linkLxr(node);
                            break;

                        case 2: assert( "SMR" == label );
                            linkSmr(node);
                            break;

                        default:
                            throw exception("gCompiler.link: unknowm \"%s\"", label.c_str());
                    }
                }

                if(verbose)
                {
                    std::cerr << std::endl;
                }
            }


            //__________________________________________________________________
            //
            //
            // linking top level rule
            //
            //__________________________________________________________________
            void gCompiler:: linkRule(const Node *node)
            {
                const Node::List &children = node->toList();
                const Node       *child    = children.head; assert(child); assert("ID"==child->origin.label);
                const string      label    = child->toString();

                if(verbose) { std::cerr << label << ':'; }

                gRule *ppR = ruleDB.search(label);
                if(!ppR)
                {
                    gTerm *ppT = termDB.search(label);
                    if(!ppT)
                    {
                        throw exception("gCompiler.linkRule: unregistered '%s'", label.c_str());
                    }
                    if(verbose) { std::cerr << " (alias);" << std::endl; }
                    return;
                }

                Aggregate &topRule = **ppR;
                for(child=child->next;child;child=child->next)
                {
                    topRule << walkRule(child);
                }
                if(verbose) { std::cerr << ';' << std::endl; }
            }

            //__________________________________________________________________
            //
            //
            // Recursive call to build parser
            //
            //__________________________________________________________________
            Rule & gCompiler:: walkRule(const Node *node)
            {
                static const char fn[] = "gCompiler.walkRule: ";
                const string label = node->origin.label;

                switch(walkHash(label))
                {
                    case 0: assert("ID"==label);
                    {
                        const  string id = node->toString();
                        if(verbose) { std::cerr << ' ' << id; }
                        return getRule(id);
                    }

                    case 1: assert("RX"==label);
                    {
                        const  string id = node->toString();
                        if(verbose) { std::cerr << " \"" << id << "\""; }
                        return getTerm(id);
                    }

                    case 2: assert("RS"==label);
                    {
                        const string id = node->toString();
                        if(verbose) { std::cerr << " \'" << id << "\'"; }
                        return getTerm(id);
                    }
                        
                    case 3: assert("SUB"==label);
                    {
                        Aggregate &sub = parser->agg( parser->newAggLabel() );
                        if(verbose) { std::cerr << '('; }
                        fillCompound(sub,node);
                        if(verbose) { std::cerr << ')'; }
                        return sub;
                    }

                    case 4: assert("ALT"==label);
                    {
                        Alternate &alt = parser->alt();
                        fillCompound(alt,node);
                        return alt;
                    }

                    case 5: assert("OPT"==label);
                    {
                        Rule &r = parser->optional(walkRule(node->head()));
                        if(verbose) { std::cerr << '?'; }
                        return r;
                    }
                    case 6: assert("OOM"==label);
                    {
                        Rule &r = parser->oneOrMore(walkRule(node->head()));
                        if(verbose) { std::cerr << '+'; }
                        return r;
                    }

                    case 7: assert("ZOM"==label);
                    {
                        Rule &r = parser->zeroOrMore(walkRule(node->head()));
                        if(verbose) { std::cerr << '*'; }
                        return r;
                    }

                    default:
                        break;

                }
                throw exception("%sunhandled '%s'",fn,label.c_str());
            }


            //__________________________________________________________________
            //
            //
            // call a terminal
            //
            //__________________________________________________________________
            Rule  &gCompiler:: getTerm(const string &name)
            {
                gTerm *ppT = termDB.search(name);
                if(!ppT)
                {
                    throw exception("gCompiler.getTerm: no '%s'", name.c_str());
                }
                return **ppT;
            }

            //__________________________________________________________________
            //
            //
            // call a rule or a terminal
            //
            //__________________________________________________________________
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


            //__________________________________________________________________
            //
            //
            // fill an innner compound rule
            //
            //__________________________________________________________________
            void gCompiler:: fillCompound(Compound   &cmp,
                                          const Node *from)
            {
                const bool  showAlt = (verbose && (Alternate::UUID == cmp.uuid));
                const Node *head    = from->head();
                for(const Node *node = head; node; node=node->next)
                {
                    if(showAlt&&(node!=head))
                    {
                        std::cerr << '|';
                    }
                    cmp << walkRule(node);

                }
            }


            //__________________________________________________________________
            //
            //
            // Lexical Only rule
            //
            //__________________________________________________________________
            void gCompiler:: linkLxr(const Node *node)
            {
                const Node::List &children = node->toList(); assert(children.size>0);
                node                       = children.head;  assert(children.head);
                const string      lx       = node->toString(1);

                if(verbose)
                {
                    std::cerr << "|@" << lx << std::endl;
                }

                const string lxr_label_base = parser->tag + '.' + lx;
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

            void gCompiler:: linkSmr(const Node *node)
            {
                node = node->head();
                assert(node!=NULL);
                assert("SM"==node->origin.label);
                const string SM = node->toString(1);
                if(verbose)
                {
                    std::cerr << "|_" << SM << ':';
                }

                if( "NoSingle" == SM )
                {
                    for(node=node->next;node;node=node->next)
                    {
                        const string id = node->toString();
                        if(verbose)
                        {
                            std::cerr << ' ' << id;
                        }
                        gRule *ppR = ruleDB.search(id);
                        if(!ppR)
                        {
                            throw exception("%s: no rule %s", *SM, *id);
                        }
                        (**ppR).let(NoSingle);
                    }
                    goto SMR_DONE;
                }

            SMR_DONE:
                if(verbose)
                {
                    std::cerr << ';' << std::endl;
                }
            }

        }
        
    }
}
