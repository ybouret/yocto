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
                //
                //______________________________________________________________
                unsigned indx=1;
                for( dynRuleDB::iterator i=ruleDB.begin(); i != ruleDB.end(); ++i,++indx)
                {
                    Aggregate &agg = **i;
                    __detectPropertiesOf(agg);
                }

                if(verbose)
                {
                    vector<string> terminals;
                    vector<string> internals;
                    parser->collectLabels(terminals,internals);
                    std::cerr << "terminals=" << terminals << std::endl;
                    std::cerr << "internals=" << internals << std::endl;

                    std::cerr.flush();
                    ios::ocstream fp(ios::cstderr);
                    const string  prefix = parser->tag + '_';
                    hashing::mperf::emit_defines(fp,terminals,prefix);
                    fflush(stderr);
                    std::cerr << std::endl;
                    hashing::mperf::emit_defines(fp,internals,prefix,0x10000);

                    {
                        string terminalsKW;
                        {
                            ios::osstream fp(terminalsKW);
                            for(size_t i=1;i<=terminals.size();++i)
                            {
                                ios::net_string::write(terminals[i],fp);
                            }
                        }
                        std::cerr << "terminal_keywords='" << terminalsKW << "'" << std::endl;
                    }

                    {
                        string internalsKW;
                        {
                            ios::osstream fp(internalsKW);
                            for(size_t i=1;i<=internals.size();++i)
                            {
                                ios::net_string::write(internals[i],fp);
                            }
                        }
                        std::cerr << "internal_keywords='" << internalsKW << "'" << std::endl;
                    }


                }
                
                termDB.release();
                ruleDB.release();
                std::cerr << std::endl;
            }
        }
    }
}
