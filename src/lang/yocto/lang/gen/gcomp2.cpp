#include "yocto/lang/gen/gcompiler.hpp"
#include "yocto/exception.hpp"

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

            void gCompiler:: collect(const Node *node)
            {
                static const char fn[] = "gCompiler.collect: ";
                assert(node);
                if(node->terminal)
                {
                    const string &label   = node->origin.label;
                    const string  content = node->toString();
                    if(verbose)
                    {
                        std::cerr << "+term '" << label << "'='" << content << "'" << std::endl;
                    }

                    switch(termHash(label))
                    {
                        case 0: assert("ID"==label);
                            if(!has(content)) throw exception("%sundeclared rule '%s'", fn, content.c_str());
                            break;

                        case 1: assert("RX"==label);
                            break;

                        case 2: assert("RS"==label);
                            break;

                        case 3: assert("RB"==label);
                            break;

                        default:
                            throw exception("%sunhandled terminal '%s'", fn, label.c_str());
                    }

                }
                else
                {
                    for(node = node->head();node;node=node->next)
                    {
                        collect(node);
                    }
                }

            }

        }
    }
}

