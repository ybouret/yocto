#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/fs/local-fs.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/lang/lexical/plugin/cstring.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            void DynamoCompiler:: __newRule(const char *fn, dynRule pR)
            {
                assert(pR);
                if(!ruleDB.insert(pR->label,pR))
                {
                    throw exception("%sunexpected multiple rule '%s'", fn, *(pR->label) );
                }
            }


            void DynamoCompiler:: __newTerm(const char *fn, dynTerm pT)
            {
                assert(pT);
                if(!termDB.insert(pT->label,pT))
                {
                    throw exception("%sunexpected multiple terminal '%s'", fn, *(pT->label) );
                }
            }




            string DynamoCompiler:: RS2Expr(const string &RS)
            {
                const size_t n = RS.size();
                string rx(4*n,as_capacity);
                for(size_t i=0;i<n;++i)
                {
                    rx << '\\' << 'x' << hexa_text_lower[uint8_t(RS[i])];
                }
                return rx;
            }

            string DynamoCompiler:: String2Expr(const Node *node) const
            {
                const string label = node->origin.label;
                switch(strHash(label))
                {
                    case 0: assert("RX"==label); return node->toString();
                    case 1: assert("RS"==label); { const string rs = node->toString(); return RS2Expr(rs); }
                    default:
                        break;
                }
                throw exception("DynamoCompiler.String2Expr: unexpected node '%s'", *label);
            }


            void DynamoCompiler:: createTopLevelFrom(Node *master)
            {
                static const char fn[] = "Dynamo.createTopLevel: ";

                //______________________________________________________________
                //
                //
                // get info from master node
                //
                //______________________________________________________________

                assert(master);
                assert(master->internal);
                Node::List &topLevel = master->toList(); assert(topLevel.size>0);

                {
                    auto_ptr<Node> nameNode( topLevel.pop_front() );
                    assert(nameNode.is_valid());
                    assert(nameNode->terminal);
                    const string   name = nameNode->toString(1);
                    if(verbose) { std::cerr << "== new parser " << name << " ==" << std::endl; }
                    parser.reset( new Parser(name) );
                    if(verbose)
                    {
                        vfs &fs = local_fs::instance();
                        const string parserDot = parser->tag+".dot";
                        const string parserGfx = parser->tag+".gfx";
                        fs.try_remove_file(parserDot);
                        fs.try_remove_file(parserGfx);

                    }
                }

                //______________________________________________________________
                //
                //
                // create top level rules and alias
                //
                //______________________________________________________________
                {
                    Node::List tmp;
                    while(topLevel.size>0)
                    {
                        auto_ptr<Node> node( topLevel.pop_front() );
                        const string   label = node->origin.label;

                        switch(topHash(label))
                        {
                                //______________________________________________
                                //
                                //
                            case 0: assert("RULE"==label);
                                //
                                //______________________________________________
                            {
                                const string ruleLabel = node->head()->toString();
                                if(verbose) { std::cerr << "+" << ruleLabel << std::endl; }
                                Aggregate   &rule      = parser->agg(ruleLabel);
                                __newRule(fn,&rule);
                            } break;


                                //______________________________________________
                                //
                                //
                            case 1 : assert("ALIAS"==label);
                                //
                                //______________________________________________
                            {
                                Node::List   &alias      = node->toList(); assert(2==alias.size);
                                const string  ruleLabel  = alias.head->toString();
                                const string &stringKind = alias.tail->origin.label;
                                const string  stringExpr = alias.tail->toString();


                                switch(strHash(stringKind))
                                {
                                    case 0: assert("RX"==stringKind);
                                    {
                                        if(verbose) { std::cerr << "*" << ruleLabel << " RX=\"" << stringExpr << "\"" << std::endl; }
                                        Terminal &t = parser->terminal(ruleLabel,stringExpr);
                                        __newTerm(fn,&t);
                                    } break;

                                    case 1: assert("RS"==stringKind);
                                    {
                                        const string ruleExpr = RS2Expr(stringExpr);
                                        if(verbose) { std::cerr << "*" << ruleLabel << " RS='" << stringExpr << "'=>\"" << ruleExpr << "\"" << std::endl; }

                                        Terminal    &t        = parser->terminal(ruleLabel,ruleExpr);
                                        __newTerm(fn,&t);
                                        t.let(IsUnique);
                                    } break;

                                    default:
                                        throw exception("%sunexpected string kind '%s'", fn, *stringKind);
                                }
                                node.release();
                            } break;


                                //______________________________________________
                                //
                                //
                            case 2 : assert("PLUG"==label);
                                //
                                //______________________________________________
                            {
                                Node::List   &plug      = node->toList(); assert(2==plug.size);
                                const string  ruleLabel = plug.head->toString(1);
                                const string  thePlugin = plug.tail->toString();
                                if(verbose) { std::cerr << "@" << ruleLabel << "=plugin<" << thePlugin << ">" << std::endl; }
                                node.release();

                                if("cstring"==thePlugin)
                                {
                                    Terminal &t = parser->term<Lexical::cstring>(ruleLabel);
                                    __newTerm(fn, &t);
                                    break;
                                }

                                if("rstring"==thePlugin)
                                {
                                    Terminal &t = parser->term<Lexical::rstring>(ruleLabel);
                                    __newTerm(fn, &t);
                                    break;
                                }

                                throw exception("%sunhandled plugin<%s>",fn,*thePlugin);

                            } break;

                                //______________________________________________
                                //
                            case 3 : assert("LEXR"==label);
                                //______________________________________________
                            {
                                // for later
                            } break;


                                //______________________________________________
                                //
                            case 4 : assert("SEMR"==label);
                                //______________________________________________
                            {
                                // for later
                            } break;

                            default:
                                throw exception("unhandled top level '%s'", *label);
                        }

                        if(node.is_valid())
                        {
                            tmp.push_back( node.yield() );
                        }
                        else
                        {
                            if(verbose) { std::cerr << "\\_deleted entry" << std::endl; }
                        }

                    }
                    topLevel.swap_with(tmp);
                }
                if(verbose) { std::cerr << std::endl; }
            }


        }

    }
}

