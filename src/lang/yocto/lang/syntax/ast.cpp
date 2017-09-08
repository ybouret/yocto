//#include "yocto/lang/syntax/node.hpp"
#include "yocto/lang/syntax/rules.hpp"
#include "yocto/error.hpp"
#include <cerrno>

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {



            Node * Node:: AST( Node *node ) throw()
            {
                if(!node) return NULL;

#if 0
                //______________________________________________________________
                //
                // let us take some information
                //______________________________________________________________
                if(!(node->origin.flags&IsHollow))
                {
                    std::cerr << "node=" << node->origin.label << std::endl;
                }
#endif

                if(node->terminal)
                {
                    //__________________________________________________________
                    //
                    // clean it up...
                    //__________________________________________________________
                    assert(Terminal::UUID==node->origin.uuid);
                    switch(node->origin.flags)
                    {
                        case IsNormal: return node;
                        case IsHollow: delete node; return NULL;
                        case IsUnique: static_cast<Lexeme *>(node->impl)->clear(); return node;
                        default:
                            break;
                    }

                    delete node;
                    libc::critical_error(EINVAL,"Syntax.Node.AST: invalid terminal flags");
                    return NULL;
                }
                else
                {
                    //__________________________________________________________
                    //
                    // check the list
                    //__________________________________________________________
                    List &children = *static_cast<List *>(node->impl);
                    {
                        //______________________________________________________
                        //
                        // store in temporary
                        //______________________________________________________
                        List  tmp;
                        while(children.size>0)
                        {
                            //__________________________________________________
                            //
                            // recursive call on the standalone node
                            //__________________________________________________
                            Node *child = AST( children.pop_front() );
                            if(!child) continue;

                            //__________________________________________________
                            //
                            // check fusion
                            //__________________________________________________
                            const uint32_t uuid  = child->origin.uuid;
                            const uint32_t flags = child->origin.flags;
                            const bool     fuse =
                            (   Optional::UUID == uuid) ||
                            (   Counting::UUID == uuid) ||
                            ( (Aggregate::UUID == uuid) && child->origin.label.has(Rule::FusionMark)) ||
                            ( (Aggregate::UUID == uuid) && (NoSingle==flags) && (1==child->size()) )  ||
                            ( (Aggregate::UUID == uuid) && (IsFusing==flags) );

                            if(fuse)
                            {
                                tmp.merge_back( *static_cast<List *>(child->impl) );
                                delete child;
                                child = NULL;
                            }

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
