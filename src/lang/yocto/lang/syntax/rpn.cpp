#include "yocto/lang/syntax/rule.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            RPN::RPN(const string &id) :
            label(id)
            {
            }

            RPN::RPN(const char  *id) :
            label(id)
            {
            }

            RPN:: ~RPN() throw()
            {
            }

            const string & RPN:: key() const throw()
            {
                return label;
            }

            void RPN:: op(const string &opID)
            {
                insert(opID,0);
            }

            void RPN:: op(const char *opID)
            {
                insert(opID,0);
            }


#if 0
            void RPN::apply( Node *node )
            {
                assert(node);
                if(node->internal)
                {
                    const string &nodeLabel = node->origin.label;
                    Node::List   &children  = node->toList();
                    //__________________________________________________________
                    //
                    // recursive call
                    //__________________________________________________________
                    for(Node *ch = children.head; ch; ch=ch->next)
                    {
                        apply(ch);
                    }
                    if(nodeLabel!=label) return;
                    std::cerr << "RPN " << label << "/" << children.size << std::endl;

                    //__________________________________________________________
                    //
                    // operator inversion
                    //__________________________________________________________
                    Node::List rpn;
                    while(children.size)
                    {
                        Node *ch = children.pop_front();
                        if((*this)(ch->origin.label)>=0)
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
#endif

            RPN_Set:: RPN_Set() throw()
            {
            }

            RPN_Set:: ~RPN_Set() throw()
            {
            }

            RPN & RPN_Set:: operator()(const string &id)
            {
                RPN::Pointer *ppRPN = search(id);
                if(ppRPN)
                {
                    return **ppRPN;
                }
                else
                {
                    RPN::Pointer pRPN( new RPN(id) );
                    if(!insert(pRPN))
                    {
                        throw exception("RPN_Set: unexpected failure for '%s'", *id);
                    }
                    return *pRPN;
                }
            }

            RPN & RPN_Set:: operator()(const char *id)
            {
                const string ID(id);
                return (*this)(ID);
            }

            void RPN_Set:: operator()( Node *node )
            {
                assert(node);
                RPN_Set &self = *this;
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
                        self(ch);
                    }

                    //__________________________________________________________
                    //
                    // check if possible
                    //__________________________________________________________
                    const RPN::Pointer *ppRPN = self.search(label);
                    if(!ppRPN) return;

                    //__________________________________________________________
                    //
                    // processing
                    //__________________________________________________________
                    const RPN &H = **ppRPN;
                    std::cerr << "RPN for " << label << std::endl;
                    Node::List rpn;
                    while(children.size)
                    {
                        Node         *ch = children.pop_front();
                        const string &op = ch->origin.label;
                        if(H(op)>=0)
                        {
                            if(children.size<=0)
                            {
                                delete ch;
                                throw exception("RPN('%s'): no operand after '%s'", *label, *op );
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

