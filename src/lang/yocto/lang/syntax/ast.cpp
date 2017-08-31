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

                //______________________________________________________________
                //
                // let us take some information
                //______________________________________________________________
                if(!(node->origin.flags&IsHollow))
                {
                    std::cerr << "node=" << node->origin.label << std::endl;
                }
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
                    throw exception("Syntax.Node.AST: invalid terminal flags=%08x",unsigned(node->origin.flags));
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
                            //std::cerr << "\tchild=" << child->origin.label << std::endl;

                            //__________________________________________________
                            //
                            // check fusion
                            //__________________________________________________
                            const uint32_t uuid = child->origin.uuid;
                            const bool     fuse =
                            (  Optional::UUID == uuid) ||
                            (  Counting::UUID == uuid) ||
                            ((Aggregate::UUID == uuid) && NULL!=strchr(child->origin.label.c_str(),Rule::FusionMark));

                            if(fuse)
                            {
                                //std::cerr << "fusion of " << child->origin.label << " with " << node->origin.label << std::endl;
                                List &sub = *static_cast<List *>(child->impl);
                                while(sub.size)
                                {
                                    Node *ch = sub.pop_front();
                                    tmp.push_back(ch);
                                }
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
