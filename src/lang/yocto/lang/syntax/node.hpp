#ifndef YOCTO_LANG_SYNTAX_NODE_INCLUDED
#define YOCTO_LANG_SYNTAX_NODE_INCLUDED 1

#include "yocto/lang/lexical/unit.hpp"
#include "yocto/ios/ostream.hpp"
#include "yocto/hashing/mph.hpp"

namespace yocto
{
    namespace Lang
    {
        typedef hashing::mperf Hasher;

        namespace Syntax
        {

            typedef Lexical::Unit  Lexeme;

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

                //! dangerous, must be called just before deletion
                Lexeme *yield() throw();

                //! append GraphViz representation
                void viz( ios::ostream &fp ) const;

                //! visualize this as root node
                void graphviz(const string &filename) const;

                //! Abstract Syntax Tree cleaning
                static Node * AST( Node *node ) throw();

                static void   RPN( Node *node, const string &id, const Hasher &ops);

                //! wrapper to get list
                List         & toList() throw();

                //! wrapper to get list
                const List   & toList() const throw();

                //! wrapper to get lexeme
                Lexeme       & toLex() throw();

                //! wrapper to get lexeme
                const Lexeme & toLex() const throw();

                //! wrapper to get first item if internal
                const Node *head() const throw();

                //! wrapper to get lexeme content if terminal
                string      toString(const size_t nskip=0) const;

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
