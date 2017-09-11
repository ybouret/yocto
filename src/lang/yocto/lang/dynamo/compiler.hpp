#ifndef YOCTO_LANG_DYNAMO_COMPILER_INCLUDED
#define YOCTO_LANG_DYNAMO_COMPILER_INCLUDED 1

#include "yocto/lang/dynamo/parser.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            class DynamoCompiler : public DynamoParser
            {
            public:
                explicit DynamoCompiler();
                virtual ~DynamoCompiler() throw();

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(DynamoCompiler);
            };

        }
        
    }
    
}


#endif
