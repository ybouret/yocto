#ifndef YOCTO_LANG_SYNTAX_NODE_INCLUDED
#define YOCTO_LANG_SYNTAX_NODE_INCLUDED 1

#include "yocto/lang/lexical/unit.hpp"
#include "yocto/lang/syntax/rpn.hpp"
#include "yocto/hashing/function.hpp"
#include "yocto/ios/ostream.hpp"
#include "yocto/ptr/zrc.hpp"

namespace yocto
{
    namespace Lang
    {

        namespace Syntax
        {

            typedef Lexical::Unit  Lexeme;

            class Rule;    //!< forward declaration
            class Grammar; //!< forward declaration for I/O
            typedef zrc_ptr<const string> Tag; //!< for Dynamo

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

                void   append(Node *child) throw();  //!< INTERNAL: append
                Node  *remove_head() throw();        //!< INTERNAL: if size>0
                Node  *remove_tail() throw();        //!< INTERNAL: if size>0
                size_t size() const throw();         //!< INTERNAL: list size

                virtual ~Node() throw();

                //! create an internal node
                static Node *Create(const Source &source, const Rule &r);

                //! create a terminal node, lexeme is taken care of
                static Node *Create(const Source &source, const Rule &r, Lexeme *l);

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
                
                //! INTERNAL: wrapper to get list
                List         & toList() throw();

                //! INTERNAL: wrapper to get list
                const List   & toList() const throw();

                //! TERMINAL: wrapper to get lexeme
                Lexeme       & toLex() throw();

                //! TERMINAL: wrapper to get lexeme
                const Lexeme & toLex() const throw();

                //! INTERNAL: wrapper to get first child
                const Node *head() const throw();

                //! INTERNAL: wrapper to get lexeme content
                string      toString(const size_t nskip=0) const;

                //! serialize binary node
                void serialize( ios::ostream &fp ) const;

                //! recurvsive fill the H function
                void runHash( hashing::function &H ) const;

                //! full hash and string conversion
                string signature() const;

                //! loading using grammar
                static Node *loadFrom(Module *module, const Grammar &G );

                //! propagate tag to children that don't already have one
                void propagate(const Tag &tag) throw();

            private:
                void                *impl;  //!< lexeme or list
                YOCTO_DISABLE_ASSIGN(Node);
                Node(const Node &other);

                //! make a non terminal node
                explicit Node(const Source &, const Rule &r);

                //! a terminal node
                explicit Node(const Source &, const Rule &r, Lexeme *l) throw();

                static Node * __loadFrom( Source &source, ios::istream &fp, const Grammar &G );

            public:
                const Stamp stamp; //!< source stamp, when created
                Tag         tag;   //!< Dynamo tag
            };


        }
    }
}

#endif
