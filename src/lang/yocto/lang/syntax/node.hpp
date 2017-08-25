#ifndef YOCTO_LANG_SYNTAX_NODE_INCLUDED
#define YOCTO_LANG_SYNTAX_NODE_INCLUDED 1

#include "yocto/lang/lexical/rule.hpp"

namespace yocto
{
    namespace Lang
    {

        namespace Syntax
        {

            class Rule; //!< forward declaration

            //! Syntax Node
            class Node : public object
            {
            public:
                typedef core::list_of_cpp<Node> List;

                Node                *next;
                Node                *prev;
                Node                *parent;
                const Rule          *origin; //!< created by this rule
                const bool           terminal;
                void                *impl;
                
                virtual ~Node() throw();
                explicit Node(const Rule &r);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Node);
            };


        }
    }
}

#endif
