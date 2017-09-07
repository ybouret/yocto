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
                    std::cerr << "== register terminals and check rules ==" << std::endl;
                }
                for(const Node *node=topNode;node;node=node->next)
                {
                    if("RULE"==node->origin.label)
                    {
                        collect(node);
                    }
                }
            }

            namespace
            {
                static const char fn[] = "gCompiler.collect: ";
            }

            Terminal & gCompiler::registerNewTermRX(const string &expr)
            {
                Terminal &t = parser->terminal(expr);
                if( !termDB.insert(expr,&t))
                {
                    throw exception("%sunexpected failure for '%s'",fn,expr.c_str());
                }
                return t;
            }

            Terminal & gCompiler::registerNewTermRS(const string &expr)
            {
                parser->root.make(expr, Logical::Equal(expr), & *parser, & Lexical::Translator::forward );
                Terminal &t = parser->add( new Terminal(expr) );
                if( !termDB.insert(expr,&t))
                {
                    throw exception("%sunexpected failure for '%s'",fn,expr.c_str());
                }
                return t;
            }


            void gCompiler:: collect(const Node *node)
            {
                assert(node);
                if(node->terminal)
                {
                    const string &label   = node->origin.label;
                    const string  content = node->toString();
                    if(verbose)
                    {
                        std::cerr << "+term '" << label << "'='" << content << "'" << std::endl;
                    }

                    switch( termHash(label) )
                    {
                        case 0: assert("ID"==label);
                            if(!has(content)) throw exception("%sundeclared '%s'",fn,content.c_str());
                            break;

                        case 1: assert("RX"==label);
                            if(!termDB.search(content))
                            {
                                registerNewTermRX(content);
                            }
                            break;

                        case 2: assert("RS"==label);
                            if(!termDB.search(content))
                            {
                                registerNewTermRS(content).let(IsHollow);
                            }
                            break;


                        default:
                            throw exception("%sunhandled '%s'",fn,label.c_str());

                    }



                }
                else
                {
                    for(node = node->head();node;node=node->next)
                    {
                        collect(node);
                    }
                }
                return;
            }
            
        }
    }
}

