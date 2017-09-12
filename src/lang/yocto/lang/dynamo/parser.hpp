#ifndef YOCTO_LANG_DYNAMO_PARSER_INCLUDED
#define YOCTO_LANG_DYNAMO_PARSER_INCLUDED 1

#include "yocto/lang/syntax/parser.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            //! define the DynamoParser
            class DynamoParser : public Parser
            {
            public:
                virtual ~DynamoParser() throw();
                explicit DynamoParser();


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(DynamoParser);
            };

        }
    }
}

#endif
