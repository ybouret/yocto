#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            void DynamoCompiler:: semr(const Node &node)
            {
                const Node::List &children = node.toList(); assert(children.size>=2);
                const Node       *ch       = children.head;
                const string      SM       = ch->toString(1);
                if(verbose)
                {
                    std::cerr << "\t\\_%" << SM << std::endl;
                }
                if(SM=="RPN")
                {
                    onRPN(ch->next);
                    return;
                }
                throw exception("DynamoCompiler: unhandled Semantic Rule %%%s",*SM);
            }

            void DynamoCompiler:: onRPN(const Node *code)
            {
                assert(code!=NULL);
                assert("ID"==code->origin.label);
                const string ruleLabel = code->toString();
                if(verbose)
                {
                    std::cerr << "\t  \\_" << ruleLabel << ':';
                }
                RPN &rpn = parser->rdb(ruleLabel);
                code=code->next;
                if(code)
                {
                    assert(code->origin.label=="ARGS");
                    for(const Node *node=code->head();node;node=node->next)
                    {
                        assert("ID"==node->origin.label);
                        const string argsLabel = node->toString();
                        if(verbose) { std::cerr << ' ' << argsLabel; }
                        rpn.op(argsLabel);
                    }
                    rpn.optimize();

                }
                if(verbose)
                {
                    std::cerr << std::endl;
                }
            }

        }
    }
}
