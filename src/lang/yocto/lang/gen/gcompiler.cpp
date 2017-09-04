#include "yocto/lang/gen/gcompiler.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            const char gCompiler::name[] = "Grammar Compiler";

            gCompiler:: ~gCompiler() throw()
            {
            }

            gCompiler:: gCompiler() :
            getAST()
            {

            }
            
        }
    }
}

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            Parser * gCompiler:: encode(const Node *ast)
            {
                auto_ptr<Parser> p;
                return p.yield();
            }

        }

    }
}
