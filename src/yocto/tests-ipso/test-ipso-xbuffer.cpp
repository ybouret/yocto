#include "yocto/ipso/xbuffer.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/hashing/sha1.hpp"

using namespace yocto;
using namespace ipso;

static inline
void display( const exchange_buffer &xbuff )
{
    std::cerr << "\tload=" << xbuff.load() << " | left=" << xbuff.left() << std::endl;
}

YOCTO_UNIT_TEST_IMPL(xbuffer)
{
    for(size_t iter=0;iter<10;++iter)
    {
        const size_t   nw = 1+alea.leq(20);
        vector<float>  vec_f(nw,as_capacity);
        vector<double> vec_d(nw,as_capacity);
        vector<int>    vec_i(nw,as_capacity);
        const size_t   num_bytes = nw * ( sizeof(float) + sizeof(double) + sizeof(int) );
        hashing::sha1 H;
        H.set();
        for(size_t i=0;i<nw;++i)
        {
            vec_f.push_back( alea.get<float>()  );
            vec_d.push_back( alea.get<double>() );
            vec_i.push_back( alea.leq(100)      );
            H.run_type(vec_f.back());
            H.run_type(vec_d.back());
            H.run_type(vec_i.back());
        }
        const uint64_t i64 = H.key<uint64_t>();
        std::cerr << "i64=" << i64 << std::endl;
        exchange_buffer xbuff(num_bytes);
        display(xbuff);
        for(size_t i=1;i<=nw;++i)
        {
            xbuff.store( vec_f[i] );
            xbuff.store( vec_d[i] );
            xbuff.store( vec_i[i] );
            display(xbuff);
        }
        
        H.set();
        for(size_t i=1;i<=nw;++i)
        {
            const float  f = xbuff.query<float>();  H.run_type(f);
            const double d = xbuff.query<double>(); H.run_type(d);
            int a=-1;
            xbuff.query(a);    H.run_type(a);
            display(xbuff);
        }
        const uint64_t o64 = H.key<uint64_t>();
        std::cerr << "o64=" << o64 << std::endl;
        if(i64!=o64)
        {
            throw exception("I/O error");
        }
    }
}
YOCTO_UNIT_TEST_DONE()

