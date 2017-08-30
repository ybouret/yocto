//#include "yocto/lang/syntax/node.hpp"
#include "yocto/lang/syntax/rules.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {


            Node * Node:: AST( Node *node ) throw()
            {
                if(!node) return NULL;
                
                const Rule    &rule  = node->origin;
                const uint32_t uuid  = rule.uuid;
                const uint32_t flags = rule.flags;
                void          *impl  = node->impl;

                if(node->terminal)
                {
                    assert(Terminal::UUID==uuid);
                    switch(flags)
                    {
                        case IsNormal: return node;
                        case IsHollow: delete node; return NULL;
                        case IsUnique: static_cast<Lexeme *>(impl)->clear(); return node;
                        default:
                            break;
                    }
                    throw exception("Syntax.Node.AST: invalid terminal flags=%08x",unsigned(flags));
                }
                else
                {
                    List &children = *static_cast<List *>(impl);
                    {
                        List  tmp;
                        while(children.size>0)
                        {
                            Node *child = AST(children.pop_front());
                            if(child)
                            {
                                tmp.push_back(child);
                            }
                        }
                        children.swap_with(tmp);
                    }
                    return node;
                }
                
                
            }
            
        }
    }
}
