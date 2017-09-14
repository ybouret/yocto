#include "yocto/lang/syntax/rule.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            void Node:: RPN( Node *node, const string &id, const Hasher &ops)
            {
                assert(node);
                if(node->internal)
                {
                    const string &label    = node->origin.label;
                    Node::List   &children = node->toList();
                    //__________________________________________________________
                    //
                    // recursive call
                    //__________________________________________________________
                    for(Node *ch = children.head; ch; ch=ch->next)
                    {
                        RPN(ch,id,ops);
                    }
                    if(label!=id) return;
                    std::cerr << "RPN " << label << "/" << children.size << std::endl;

                    //__________________________________________________________
                    //
                    // operator inversion
                    //__________________________________________________________
                    Node::List rpn;
                    while(children.size)
                    {
                        Node *ch = children.pop_front();
                        if(ops(ch->origin.label)>=0)
                        {
                            if(children.size<=0)
                            {
                                delete ch;
                                throw exception("Node.RPN: no operand after '%s'", ch->origin.label.c_str() );
                            }
                            rpn.push_back(children.pop_front());
                            rpn.push_back(ch);
                        }
                        else
                        {
                            rpn.push_back(ch);
                        }
                    }
                    children.swap_with(rpn);

                }
            }

        }
    }
}

