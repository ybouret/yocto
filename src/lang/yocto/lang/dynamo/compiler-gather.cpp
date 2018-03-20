#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"
#include "yocto/ios/graphviz.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            Aggregate &DynamoCompiler:: findRule(const char *fn, const string &label)
            {
                dynRule *ppR = ruleDB.search(label);
                if(!ppR)
                {
                    throw exception("%sunregistered rule '%s'", fn, *label);
                }
                return **ppR;
            }

            Terminal &DynamoCompiler:: findTerm(const char *fn, const string &label)
            {
                dynTerm *ppT = termDB.search(label);
                if(!ppT)
                {
                    throw exception("%sunregistered terminal '%s'", fn, *label);
                }
                return **ppT;
            }

            Rule & DynamoCompiler:: find(const char *fn, const string &label)
            {
                dynRule *ppR = ruleDB.search(label);
                if(!ppR)
                {
                    dynTerm *ppT = termDB.search(label);
                    if(!ppT)
                    {
                        throw exception("%sunregistered rule or terminal '%s'", fn, *label);
                    }
                    return **ppT;
                }
                return **ppR;
            }

            static
            inline void ScanAgg( Aggregate &agg, const bool verbose)
            {
                MetaList &members = agg.members;
                if(members.size<=1) return;

                for(MetaNode *m = agg.members.head; m; m=m->next)
                {
                    Rule *rule = m->addr;
                    if( Terminal::UUID == rule->uuid )
                    {
                        if(rule->flags==IsUnique)
                        {
                            rule->let(IsHollow);
                            if(verbose) { std::cerr << "/*'" << rule->label << "'.hollow*/"; }
                        }
                    }
                }

            }

            static inline
            void OptimizeTopRule( Grammar &G, Aggregate &agg, const bool verbose )
            {
                MetaList &members = agg.members;
                if(1==members.size)
                {
                    Rule *rule = members.head->addr;
                    if (Aggregate::UUID==rule->uuid)
                    {
                        Aggregate &sub = *static_cast<Aggregate *>(rule->self_);
                        if(verbose)
                        {
                            std::cerr << "\t//" << rule->label << "-->" << agg.label << std::endl;
                        }
                        members.swap_with(sub.members);
                        G.__suppress(rule);
                    }
                }
            }


            void DynamoCompiler:: gatherFrom(Node *master)
            {
                static const char fn[] = "DynamoCompiler.gather: ";

                assert(master);
                assert(master->internal);
                Node::List &topLevel = master->toList();
                if(verbose) { std::cerr << "== gathering " << parser->tag << " ==" << std::endl; }

                {
                    Node::List tmp;
                    while(topLevel.size>0)
                    {
                        auto_ptr<Node> node( topLevel.pop_front() );
                        const string  &label = node->origin.label;

                        switch(topHash(label))
                        {
                                //______________________________________________
                                //
                                //
                            case 0: assert("RULE"==label);
                                //
                                //______________________________________________
                            {
                                const Node::List &ruleDecl = node->toList();
                                const Node       *sub      = ruleDecl.head;
                                const string      ruleName = sub->toString();
                                if(verbose) { std::cerr << ruleName << ':'; }
                                Aggregate        &topRule  = findRule(fn,ruleName);
                                for(sub=sub->next;sub;sub=sub->next)
                                {
                                    topRule << walk(sub);
                                }
                                if(verbose) { std::cerr << ";" << std::endl; }
                                node.release();
                                // need to optimize top rule!
                                OptimizeTopRule(*parser,topRule,verbose);
                                ScanAgg(topRule,verbose);
                            }  break;


                                //______________________________________________
                                //
                                //
                            case 3 : assert("LEXR"==label);
                                //
                                //______________________________________________
                            {
                                lexr(*node);
                                node.release();
                            } break;

                                //______________________________________________
                                //
                                //
                            case 4: assert("SEMR"==label);
                                //
                                //______________________________________________
                            {
                                semr(*node);
                                node.release();
                            } break;

                            default:
                                throw exception("%sunexpected %s",fn,*label);

                        }

                        if(node.is_valid())
                        {
                            tmp.push_back( node.yield() );
                        }

                    }
                    topLevel.swap_with(tmp);
                }

                if(verbose) {
                    std::cerr << std::endl;
                }
            }

            Rule & DynamoCompiler:: walk(const Node *node)
            {
                static const char fn[] = "DynamoCompiler.walk: ";

                const string &label = node->origin.label;

                switch(lnkHash(label))
                {
                        //______________________________________________________
                        //
                        //
                    case 0: assert("ID"==label); {
                        //
                        //______________________________________________________
                        const string ID = node->toString();
                        if(verbose) { std::cerr << ' ' << ID; }
                        return find(fn,ID);
                    }

                        //______________________________________________________
                        //
                        //
                    case 1: assert("RX"==label);{
                        //
                        //______________________________________________________

                        const string RX = node->toString();
                        if(verbose) { std::cerr << ' ' << '\"' << RX << '\"'; }
                        dynTerm *ppT = termDB.search(RX);
                        if(ppT)
                        {
                            return **ppT;
                        }
                        else
                        {
                            Terminal &t = parser->terminal(RX);
                            __newTerm(fn,&t,node->reserved);
                            return t;
                        }
                    } break;

                        //______________________________________________________
                        //
                        //
                    case 2: assert("RS"==label);
                        //
                        //______________________________________________________
                    {
                        const string RS  = node->toString();
                        if(verbose) { std::cerr << ' ' << '\'' << RS << '\''; }
                        dynTerm     *ppT = termDB.search(RS);
                        if(ppT)
                        {
                            return **ppT;
                        }
                        else
                        {
                            const string EX = RS2Expr(RS);
                            Terminal    &t  = parser->terminal(RS,EX);
                            __newTerm(fn,&t,node->reserved);
                            t.let(IsUnique); // check hollow if not alone in sub-expression!
                            return t;
                        }
                    }

                        //______________________________________________________
                        //
                        //
                    case 3: assert("SUB"==label); {
                        //
                        //______________________________________________________
                        const Node::List &children = node->toList();
                        Aggregate         &r        = Decl(parser->agg( parser->newAggLabel() ),MergesAlways);

                        if(verbose) { std::cerr << '('; }
                        for(const Node *sub = children.head; sub; sub=sub->next)
                        {
                            r << walk(sub);
                        }
                        if(verbose) { std::cerr << ' ' << ')'; }
                        ScanAgg(r,verbose);
                        return r;
                    }

                        //______________________________________________________
                        //
                        //
                    case 4: assert("ALT"==label); {
                        //
                        //______________________________________________________
                        const Node::List &children = node->toList();
                        Alternate        &r        = parser->alt();

                        if(verbose) { std::cerr << '('; }
                        for(const Node *sub = children.head; sub; sub=sub->next)
                        {
                            if(verbose&&sub!=children.head) { std::cerr << ' ' << '|'; }
                            r << walk(sub);
                        }
                        if(verbose) { std::cerr << ' ' << ')'; }

                        return r;

                    }

                        //______________________________________________________
                        //
                        //
                    case 5: assert("ZOM"==label); {
                        //
                        //______________________________________________________
                        Rule &r = walk(node->head());
                        if(verbose) { std::cerr << '*'; }
                        return parser->zeroOrMore(r);
                    }

                        //______________________________________________________
                        //
                        //
                    case 6: assert("OOM"==label); {
                        //
                        //______________________________________________________
                        Rule &r = walk(node->head());
                        if(verbose) { std::cerr << '+'; }
                        return parser->oneOrMore(r);
                    }

                        //______________________________________________________
                        //
                        //
                    case 7: assert("OPT"==label); {
                        //
                        //______________________________________________________
                        Rule &r = walk(node->head());
                        if(verbose) { std::cerr << '?'; }
                        return parser->optional(r);
                    }
                }

                throw exception("%sunexpected %s",fn,*label);
            }


          

          
        }
    }
}
