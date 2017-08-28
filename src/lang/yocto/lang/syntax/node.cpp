#include "yocto/lang/syntax/node.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            Node:: ~Node() throw()
            {
                assert(impl);
                if(terminal)
                {
                    delete (Lexeme *)impl;
                }
                else
                {
                    delete (List  *)impl;
                }
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

            void Grow( Node * &tree, Node *child ) throw()
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


