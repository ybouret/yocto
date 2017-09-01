#ifndef YOCTO_LANG_SYNTAX_NODE_INCLUDED
#define YOCTO_LANG_SYNTAX_NODE_INCLUDED 1

#include "yocto/lang/lexical/unit.hpp"
#include "yocto/ios/ostream.hpp"

namespace yocto
{
    namespace Lang
    {

        namespace Syntax
        {

            typedef Lexical::Unit Lexeme;
            class Rule; //!< forward declaration

            //! a node to build an AST
            class Node : public object
            {
            public:
                typedef core::list_of_cpp<Node> List;

                Node                *next;
                Node                *prev;
                const Rule          &origin;    //!< created by this rule
                const bool           terminal;  //!< whick kind of node
                const bool           internal;  //!< which kind of node
                
                void   append(Node *child) throw();
                Node  *remove_head() throw(); //!< size>0
                Node  *remove_tail() throw();
                size_t size() const throw();

                virtual ~Node() throw();

                //! create an internal node
                static Node *Create(const Rule &r);

                //! create a terminal node, lexeme is taken care of
                static Node *Create(const Rule &r, Lexeme *l);

                //! update tree
                static void Grow( Node * &tree, Node *child ) throw();

                Lexeme *yield() throw();

                void viz( ios::ostream &fp ) const;
                void graphviz(const string &filename) const;

                //! AST
                static Node * AST( Node *node ) throw();

            private:
                void                *impl;
                YOCTO_DISABLE_COPY_AND_ASSIGN(Node);
                //! make a non terminal node
                explicit Node(const Rule &r);

                //! a terminal node
                explicit Node(const Rule &r, Lexeme *l) throw();
            };


        }
    }
}

#endif
