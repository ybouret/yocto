
#include "yocto/associative/hlist.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/core/node.hpp"
#include "yocto/string.hpp"
#include "support.hpp"
#include "yocto/sequence/vector.hpp"

using namespace yocto;

typedef core::dnode_of<string> dNode;

YOCTO_UNIT_TEST_IMPL(HList)
{
    hlist<string,dNode> hl;
    vector<string> keys;
    for(size_t i=0;i<100000;++i)
    {
        const string k = gen<string>::get();
        if(hl.insert(k, new dNode(i) ))
        {
            keys.push_back(k);
        }
        const size_t nk = keys.size();
        if(nk)
        {
            alea.shuffle(&keys[1],keys.size());
            while( keys.size() > nk/2 )
            {
                if( ! hl.remove( keys.back() ) )
                {
                    throw exception("not found '%s'", * keys.back() );
                }
                keys.pop_back();
            }
        }

    }
}
YOCTO_UNIT_TEST_DONE()

