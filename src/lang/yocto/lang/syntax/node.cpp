#include "yocto/lang/syntax/node.hpp"
#include "yocto/ptr/auto.hpp"

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

            Node:: Node(const Source &source, const Rule &r) :
            next(0),
            prev(0),
            origin(r),
            terminal(false),
            internal(true),
            impl( new List() ),
            stamp( source.stamp() ),
            reserved(NULL)
            {
                //std::cerr << "+Node(" << origin.label << ")/internal" << std::endl;
            }

            Node * Node:: Create(const Source &source, const Rule &r)
            {
                return new Node(source,r);
            }


            Node:: Node(const Source &source, const Rule &r, Lexeme *l) throw() :
            next(0),
            prev(0),
            origin(r),
            terminal(true),
            internal(false),
            impl(l),
            stamp(source.stamp()),
            reserved(NULL)
            {
                //std::cerr << "+Node(" << origin.label << ")/terminal" << std::endl;
            }


            Node * Node::Create(const Source &source, const Rule &r, Lexeme *l)
            {
                assert(NULL!=l);
                try
                {
                    return new Node(source,r,l);
                }
                catch(...)
                {
                    delete l;
                    throw;
                }

            }

            Node:: Node(const Node &other) :
            next(0),
            prev(0),
            origin(other.origin),
            terminal(other.terminal),
            internal(other.terminal),
            impl(NULL),
            stamp(other.stamp),
            reserved(other.reserved)
            {
                if(terminal)
                {
                    impl = new Lexeme( other.toLex() );
                }
                else
                {
                    auto_ptr<List> children( new List() );
                    for(const Node *node = other.head();node;node=node->next)
                    {
                        children->push_back( new Node(*node) );
                    }
                    impl = children.yield();
                }
            }


            Node::List & Node:: toList() throw()
            {
                assert(internal);
                assert(impl);
                return *static_cast<List*>(impl);
            }

            const Node::List & Node:: toList() const throw()
            {
                assert(internal);
                assert(impl);
                return *static_cast<const List*>(impl);
            }

            Lexeme & Node:: toLex() throw()
            {
                assert(terminal);
                assert(impl);
                return *static_cast<Lexeme*>(impl);

            }

            const Lexeme & Node:: toLex() const throw()
            {
                assert(terminal);
                assert(impl);
                return *static_cast<const Lexeme*>(impl);
            }


            const Node * Node:: head() const throw()
            {
                return toList().head;
            }

            string Node:: toString(const size_t nskip) const
            {
                return toLex().toString(nskip);
            }

            void Node:: append(Node *child) throw()
            {
                assert(child);
                assert(internal);

                static_cast<List *>(impl)->push_back(child);
            }

            Node  * Node:: remove_tail() throw()
            {
                assert(internal);
                assert(impl);

                List *tree = static_cast<List *>(impl);
                assert(tree->size>0);
                return tree->pop_back();
            }

            Node  * Node:: remove_head() throw()
            {
                assert(internal);
                assert(impl);

                List *tree = static_cast<List *>(impl);
                assert(tree->size>0);
                return tree->pop_front();
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

            void Node:: propagate(const Tag &tag) throw()
            {
                return;
                assert(tag.is_valid());
                if(reserved.is_valid())
                {
                    return;
                }
                else
                {
                    reserved = tag;
                    if(internal)
                    {
                        for(Node *node = static_cast<List *>(impl)->head;node;node=node->next)
                        {
                            node->propagate(tag);
                        }
                    }
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
                    const Lexeme *lex = (const Lexeme *)impl;
                    if(lex->size>0)
                    {
                        fp << '=' << '\'';
                        for(const Char *ch = lex->head;ch;ch=ch->next)
                        {
                            fp << ch->text();
                        }
                        fp << '\'';
                    }
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


