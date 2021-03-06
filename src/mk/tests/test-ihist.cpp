#include "yocto/math/stat/int-hist.hpp"
#include "yocto/utest/run.hpp"

#include "yocto/sequence/vector.hpp"
#include "yocto/sequence/list.hpp"

#include "yocto/code/alea.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(ihist)
{
    vector<int> data;
    for(size_t j=10+alea.leq(200);j>0;--j)
    {
        data.push_back( int(alea.leq(100)) - 50 );
    }
    
    list<int>      bins;
    vector<size_t> H;
    
    math::i_histogram(bins, H, data);
    
    {
        ios::ocstream fp("ihist.dat",false);
        
        size_t j=1;
        for( list<int>::iterator i=bins.begin();i!=bins.end();++i,++j)
        {
            fp("%d %u\n", *i, unsigned(H[j]));
        }
    }
    
    
    {
        ios::ocstream fp("otsu.dat",false);
        const size_t np = bins.size()+1;
        for(size_t t=0;t<=np;++t)
        {
            const double score = math::kernel::OtsuScore(bins,H,t);
            fp("%u %g\n", unsigned(t), score);
        }
    }

    std::cerr << "Threshold=" << math::OtsuThreshold(bins, H) << std::endl;

}
YOCTO_UNIT_TEST_DONE()
