#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ios/osstream.hpp"
#include "yocto/ios/net-string.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            void DynamoCompiler:: __detectPropertiesOf( Aggregate &agg )
            {
                const MetaList &members = agg.members;
                const MetaNode *node    = members.head;
                if(verbose)
                {
                    std::cerr << "===> " << agg.label;
                    for(size_t i=agg.label.size();i<=top_max_size;++i) { std::cerr << ' '; }
                }


                if(members.size==1)
                {
                    assert(node);
                    if(Alternate::UUID==node->addr->uuid)
                    {
                        if(verbose) { std::cerr << ".MergesAlways" << std::endl; }
                        agg.let(MergesAlways);
                        return;
                    }
                }

                if(1==agg.getMinCount())
                {
                    if(verbose) { std::cerr << ".MergesSingle" << std::endl; }
                    agg.let(MergesSingle);
                    return;
                }

                if(verbose) { std::cerr << std::endl; }

            }

            void DynamoCompiler:: finalize()
            {
                if(verbose)
                {
                    std::cerr << "== finalizing " << parser->tag << " ==" << std::endl;
                }

                //______________________________________________________________
                //
                // checking grammar connectivity
                //______________________________________________________________
                parser->check();

                //______________________________________________________________
                //
                // detect properties
                //______________________________________________________________
                unsigned indx=1;
                for( dynRuleDB::iterator i=ruleDB.begin(); i != ruleDB.end(); ++i,++indx)
                {
                    Aggregate &agg = **i;
                    __detectPropertiesOf(agg);
                }


                termDB.release();
                ruleDB.release();
                if(verbose) { std::cerr << std::endl; }
            }
        }
    }
}
