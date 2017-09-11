#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/exception.hpp"


namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            void DynamoCompiler:: finalize()
            {
                termDB.release();
                ruleDB.release();
            }
        }
    }
}
