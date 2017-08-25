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

        }
    }
}


