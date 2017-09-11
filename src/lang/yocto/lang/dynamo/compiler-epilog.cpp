#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/exception.hpp"


namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            static inline
            void DetectFusion( Aggregate &agg, const bool verbose )
            {
                const MetaList &members = agg.members;
                const MetaNode *node    = members.head;

                if(members.size==1)
                {
                    assert(node);
                    if(Alternate::UUID==node->addr->uuid)
                    {
                        if(verbose) { std::cerr << "//" << agg.label << ".MergesAlways" << std::endl; }
                        agg.let(MergesAlways);
                        return;
                    }
                }
            }

            void DynamoCompiler:: finalize()
            {
                if(verbose)
                {
                    std::cerr << "== finalizing " << parser->tag << " ==" << std::endl;
                }
                //______________________________________________________________
                //
                //
                //______________________________________________________________
                unsigned indx=1;
                for( dynRuleDB::iterator i=ruleDB.begin(); i != ruleDB.end(); ++i,++indx)
                {
                    Aggregate &agg = **i;
                    if(verbose)
                    {
                        std::cerr << "scanning '" << agg.label << "'" << std::endl;
                    }
                    if(indx>1)
                    {
                        DetectFusion(agg,verbose);
                    }
                }


                termDB.release();
                ruleDB.release();
                std::cerr << std::endl;
            }
        }
    }
}
