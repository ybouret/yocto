#ifndef YOCTO_LANG_SYNTAX_OPS_INCLUDED
#define YOCTO_LANG_SYNTAX_OPS_INCLUDED 1


#include "yocto/lang/syntax/node.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            struct Ops
            {
                static void Rotate3(Node        *parent,
                                    const string &op);
            };
        }
    }
}

#endif
