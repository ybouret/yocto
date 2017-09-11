#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/exception.hpp"


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


            void DynamoCompiler:: gatherFrom(Node *master)
            {
                static const char fn[] = "DynamoCompiler.gather: ";

                assert(master);
                assert(master->internal);
                Node::List &topLevel = master->toList();
                if(verbose) { std::cerr << "== gathering " << parser->tag << ", remaining#topLevel=" << topLevel.size << " ==" << std::endl; }

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
                                    topRule << compile(sub);
                                }
                                std::cerr << ";" << std::endl;
                                node.release();
                            }  break;


                                //______________________________________________
                                //
                                //
                            case 3 : assert("LEXR"==label);
                                //
                                //______________________________________________
                            {

                            } break;

                                //______________________________________________
                                //
                                //
                            case 4: assert("SEMR"==label);
                                //
                                //______________________________________________
                            {

                            } break;

                            default:
                                throw exception("%sunexpected %s",fn,*label);

                        }

                        if(node.is_valid())
                        {
                            tmp.push_back( node.yield() );
                        }
                        else
                        {
                            if(verbose) { std::cerr << "\\_removed" << std::endl; }
                        }
                    }
                    topLevel.swap_with(tmp);
                }

                if(verbose) { std::cerr << std::endl; }
            }

            Rule & DynamoCompiler:: compile(const Node *node)
            {
                const string &label = node->origin.label;
                std::cerr << "content='" << label << "'" << std::endl;
                throw exception("No Implemented");
            }
        }
    }
}
