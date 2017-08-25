#ifndef YOCTO_LANG_SYNTAX_SNODE_INCLUDED
#define YOCTO_LANG_SYNTAX_SNODE_INCLUDED 1

#include "yocto/lang/lexical/rule.hpp"

namespace yocto
{
    namespace Lang
    {

        namespace Syntax
        {

            //! Syntax Node
            class SNode : public object
            {
            public:
                SNode *next;
                SNode *prev;
                virtual ~SNode() throw();
                explicit SNode() throw();

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(SNode);
            };

            typedef core::list_of_cpp<SNode> STreeBase;

            

        }
    }
}

#endif
