#include "yocto/mpl/rational.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/alea.hpp"
#include <cstdio>

using namespace yocto;
using namespace mpl;

YOCTO_UNIT_TEST_IMPL(hash)
{
    mpq::key_hasher<hashing::sfh>  hf;
    mpq::key_hasher<hashing::sha1> h1;

    for(size_t i=1;i<=10;++i)
    {
        mpn n = alea.full<uint32_t>();
        mpz z = integer_t(alea.full<uint32_t>())*( (alea.get<float>()<0.5f) ? -1 : 1 );
        mpq q(integer_t(alea.full<uint32_t>())*( (alea.get<float>()<0.5f) ? -1 : 1 ),1+alea.full<uint32_t>());
        std::cerr << "n=" << n << std::endl;
        std::cerr << "z=" << z << std::endl;
        std::cerr << "q=" << q << std::endl;

        std::cerr.flush();
        {
            {
                const mpn           nc = n;
                {
                    const unsigned long hn  = hf(n);
                    const unsigned long hnc = hf(nc);
                    fprintf(stderr,"\t0x%08lx/0x%08lx\n",hn,hnc);
                }
                {
                    const unsigned long hn  = h1(n);
                    const unsigned long hnc = h1(nc);
                    fprintf(stderr,"\t0x%08lx/0x%08lx\n",hn,hnc);
                }
            }

            {
                const mpz           zc = z;
                {
                    const unsigned long hn  = hf(z);
                    const unsigned long hnc = hf(zc);
                    fprintf(stderr,"\t0x%08lx/0x%08lx\n",hn,hnc);
                }
                {
                    const unsigned long hn  = h1(z);
                    const unsigned long hnc = h1(zc);
                    fprintf(stderr,"\t0x%08lx/0x%08lx\n",hn,hnc);
                }
            }

            {
                const mpq           qc = q;
                {
                    const unsigned long hn  = hf(q);
                    const unsigned long hnc = hf(qc);
                    fprintf(stderr,"\t0x%08lx/0x%08lx\n",hn,hnc);
                }
                {
                    const unsigned long hn  = h1(q);
                    const unsigned long hnc = h1(qc);
                    fprintf(stderr,"\t0x%08lx/0x%08lx\n",hn,hnc);
                }
            }



        }


        std::cerr << std::endl;
    }

}
YOCTO_UNIT_TEST_DONE()
