#include "yocto/lang/syntax/node.hpp"

//#include <iostream>
//#include "yocto/lang/syntax/rule.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            Node:: ~Node() throw()
            {
                //std::cerr << "~Node(" << origin.label << ")" << std::endl;
                if(terminal)
                {
                    if(impl)
                    {
                        delete (Lexeme *)impl;
                    }
                }
                else
                {
                    assert(impl);
                    delete (List  *)impl;
                }
            }

            Lexeme * Node::yield() throw()
            {
                assert(impl);
                Lexeme *lex = (Lexeme *)impl;
                impl = 0;
                return lex;
            }

            Node:: Node(const Rule &r) :
            next(0),
            prev(0),
            parent(0),
            origin(r),
            terminal(false),
            internal(true),
            impl( new List() )
            {
                //std::cerr << "+Node(" << origin.label << ")/internal" << std::endl;
            }

            Node * Node:: Create(const Rule &r)
            {
                return new Node(r);
            }


            Node:: Node(const Rule &r, Lexeme *l) throw() :
            next(0),
            prev(0),
            parent(0),
            origin(r),
            terminal(true),
            internal(false),
            impl(l)
            {
                //std::cerr << "+Node(" << origin.label << ")/terminal" << std::endl;
            }

            Node * Node::Create(const Rule &r, Lexeme *l)
            {
                assert(NULL!=l);
                try
                {
                    return new Node(r,l);
                }
                catch(...)
                {
                    delete l;
                    throw;
                }

            }

            void Node:: append(Node *child) throw()
            {
                assert(child);
                assert(internal);

                static_cast<List *>(impl)->push_back(child);
                child->parent = this;
            }

            Node  * Node:: remove() throw()
            {
                assert(internal);
                assert(impl);

                List *tree = static_cast<List *>(impl);
                assert(tree->size>0);
                tree->tail->parent = 0;
                return tree->pop_back();
            }


            size_t Node:: size() const throw()
            {
                if(internal)
                {
                    return static_cast<List *>(impl)->size;
                }
                else
                {
                    return 0;
                }
            }

            void Node::Grow( Node * &tree, Node *child ) throw()
            {
                assert(child);
                if(tree)
                {
                    assert(tree->internal);
                    tree->append(child);
                }
                else
                {
                    tree = child;
                }
            }



        }
    }
}

#include "yocto/lang/syntax/rule.hpp"
#include "yocto/ios/ocstream.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            void Node:: viz( ios::ostream &fp ) const
            {
                assert(impl);
                fp.viz(this);
                fp("[shape=%s,label=\"%s",origin.__shp(),origin.label.c_str());
                if(terminal)
                {
                    fp << '=' << '\'';
                    const Lexeme *lex = (const Lexeme *)impl;
                    for(const Char *ch = lex->head;ch;ch=ch->next)
                    {
                        fp << ch->text();
                    }
                    fp << '\'';
                }
                fp("\"];\n");
                if(internal)
                {
                    const List *children = (const List *)impl;
                    int         iSub     = 1;
                    for(const Node *sub = children->head;sub;sub=sub->next,++iSub)
                    {
                        sub->viz(fp);
                        fp.viz(this);fp << "->";fp.viz(sub);
                        if(children->size>1)
                        {
                            fp("[label=\"%d\"]",iSub);
                        }
                        fp << ";\n";
                    }
                }
            }

            void Node:: graphviz(const string &filename) const
            {
                ios::wcstream fp(filename);
                fp << "digraph G {\n";
                viz(fp);
                fp << "}\n";
            }
        }
    }
}
