
#include "yocto/associative/hlist.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/core/node.hpp"
#include "yocto/string.hpp"
#include "support.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;

typedef core::dnode_of<string> dNode;

YOCTO_UNIT_TEST_IMPL(HList)
{
    hlist<string,dNode> hl;
    vector<string>      keys;
    size_t              nmax = 1000;
    if(argc>1)
    {
        nmax = strconv::to_size(argv[1],"nmax");
    }
    
    std::cerr << "creating " << nmax << " nodes" << std::endl;
    for(size_t i=0;i<nmax;++i)
    {
        const string k = gen<string>::get();
        if(hl.insert(k, new dNode(k) ))
        {
            keys.push_back(k);
        }
    }
    std::cerr << "hlist.size=" << hl.size() << std::endl;
    
    {
        const size_t nk = keys.size();
        if(nk>0)
        {
            std::cerr << "\tshuffling " << nk << " keys" << std::endl;
            alea.shuffle(&keys[1],keys.size());
            std::cerr << "\tremoving nodes" << std::endl;
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
    std::cerr << "hlist.size=" << hl.size() << std::endl;

}
YOCTO_UNIT_TEST_DONE()

