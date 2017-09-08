#include "yocto/lang/gen/gcompiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/lang/pattern/logic.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {


            void gCompiler:: registerTermsAndCheckRules(const Node *topNode)
            {
                if(verbose)
                {
                    std::cerr << "== register terminals (and check rules) ==" << std::endl;
                }

                for(const Node *node=topNode;node;node=node->next)
                {

                    if("RULE"==node->origin.label)
                    {
                        //______________________________________________________
                        //
                        // take care only of full rules
                        //______________________________________________________
                        const string ID = node->head()->toString();
                        if(ruleDB.search(ID))
                        {
                            // this is a rule
                            if(verbose) { std::cerr << "**  " << ID << std::endl; }
                            __collect(node);
                        }
                        else
                        {
                            // this is an alias
                            if(verbose) { std::cerr << "*** " << ID << std::endl; }
                        }
                    }
                }

                if(verbose)
                {
                    std::cerr << std::endl;
                }
            }

            namespace
            {
                static const char fn[] = "gCompiler.collect: ";
            }

            Terminal & gCompiler::registerNewTermRX(const string &expr)
            {
                Terminal &t = parser->terminal(expr);
                __newTerm(fn,expr,&t);
                return t;
            }

            Terminal & gCompiler::registerNewTermRS(const string &expr)
            {
                parser->root.make(expr, Logical::Equal(expr), & *parser, & Lexical::Translator::forward );
                Terminal &t = parser->add( new Terminal(expr) );
                __newTerm(fn,expr,&t);
                return t;
            }


            void gCompiler:: __collect(const Node *node)
            {
                assert(node);
                if(node->terminal)
                {
                    //__________________________________________________________
                    //
                    // check rule existence or register terminal
                    //__________________________________________________________
                    const string &label   = node->origin.label;
                    const string  content = node->toString();


                    switch( termHash(label) )
                    {
                        case 0: assert("ID"==label);
                            if(!has(content)) throw exception("%sundeclared '%s'",fn,content.c_str());
                            break;

                        case 1: assert("RX"==label);
                            if(!termDB.search(content))
                            {
                                if(verbose) { std::cerr << "|_RX=\"" << content << "\"" << std::endl; }
                                registerNewTermRX(content);
                            }
                            break;

                        case 2: assert("RS"==label);
                            if(!termDB.search(content))
                            {
                                if(verbose) { std::cerr << "|_RS=\'" << content << "\'" << std::endl; }
                                registerNewTermRS(content).let(IsHollow);
                            }
                            break;


                        default:
                            throw exception("%sunhandled '%s'",fn,label.c_str());

                    }



                }
                else
                {
                    //__________________________________________________________
                    //
                    // recursive call
                    //__________________________________________________________
                    for(node = node->head();node;node=node->next)
                    {
                        __collect(node);
                    }
                }
                return;
            }
            
        }
    }
}

