#ifndef YOCTO_LANG_SYNTAX_NODE_INCLUDED
#define YOCTO_LANG_SYNTAX_NODE_INCLUDED 1

#include "yocto/lang/lexical/unit.hpp"

namespace yocto
{
    namespace Lang
    {

        namespace Syntax
        {

            typedef Lexical::Unit Lexeme;
            class Rule; //!< forward declaration

            //! a node to build an AST, terminal or not
            class Node : public object
            {
            public:
                typedef core::list_of_cpp<Node> List;

                Node                *next;
                Node                *prev;
                Node                *parent;    //!< for tree operation
                const Rule          &origin;    //!< created by this rule
                const bool           terminal;  //!< whick kind of node
                const bool           internal;  //!< which kind of node
                


                virtual ~Node() throw();

                //! create an internal node
                static Node *Create(const Rule &r);

                //! create a terminal node, lexeme is taken care of
                static Node *Create(const Rule &r, Lexeme *l);

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
