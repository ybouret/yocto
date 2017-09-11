#ifndef YOCTO_LANG_DYNAMO_COMPILER_INCLUDED
#define YOCTO_LANG_DYNAMO_COMPILER_INCLUDED 1

#include "yocto/lang/dynamo/parser.hpp"
#include "yocto/associative/map.hpp"
#include "yocto/hashing/mph.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            typedef Aggregate          *dynRule;
            typedef Terminal           *dynTerm;
            typedef map<string,dynRule> dynRuleDB;
            typedef map<string,dynTerm> dynTermDB;

            class DynamoCompiler : public DynamoParser
            {
            public:


                auto_ptr<Parser> parser;
                bool             verbose;
                dynRuleDB        ruleDB;
                dynTermDB        termDB;
                hashing::mperf   topHash; //!< RULE, ALIAS, PLUG, LEXR, SEMR
                hashing::mperf   strHash; //!< RX, RS
                hashing::mperf   lnkHash; //!< ID, RX, RS, SUB, ALT, ZOM, OOM, OPT
                explicit DynamoCompiler();
                virtual ~DynamoCompiler() throw();

                Parser *encode( Node *master );


                //______________________________________________________________
                //
                // prolog
                //______________________________________________________________
                void createTopLevelFrom( Node *master );
                void  __newRule(const char *fn, dynRule pR );
                void  __newTerm(const char *fn, dynTerm pT );

                //______________________________________________________________
                //
                // gather
                //______________________________________________________________
                void     gatherFrom( Node *master );
                Aggregate &findRule(const char *fn, const string &label);
                Terminal  &findTerm(const char *fn, const string &label);
                Rule      &find(const char *fn, const string &label);
                Rule      &walk(const Node *node);
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(DynamoCompiler);
                static string RS2Expr(const string &RS);



            };

        }
        
    }
    
}


#endif
