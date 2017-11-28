#include "yocto/mpl/integer.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/alea.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/sort/quick.hpp"

using namespace yocto;
using namespace mpl;

#define _SHOW(X) std::cerr << #X " = " << X << std::endl

YOCTO_UNIT_TEST_IMPL(mpz)
{

    {
        for(integer_t i=-1000;i<=1000;i+=500)
        {
            mpz z = i;
            _SHOW(z);
        }
    }

    std::cerr << "-- Comparision Tests" << std::endl;
    {
        vector<mpz>       Z;
        vector<integer_t> z;
        for(size_t i=0;i<10000;++i)
        {
            const integer_t w = alea.full<integer_t>();
            const mpz       W = w;
            z.push_back(w);
            Z.push_back(W);
        }
        alea.shuffle(z(),z.size());
        alea.shuffle(Z(),Z.size());
        quicksort(z);
        quicksort(Z);
        for(size_t i=z.size();i>0;--i)
        {
            //std::cerr << "z=" << z[i] << "/ Z=" << Z[i] << std::endl;
            if(z[i]!=Z[i]) throw exception("invalid comparison!");
        }
    }
    
    std::cerr << "-- Addition Tests" << std::endl;
    {
        for(integer i=-5;i<=5;++i)
        {
            std::cerr << "i=" << i << std::endl;
        }
        for(size_t i=0;i<10000;++i)
        {
            const integer_t x = alea.full<int32_t>();
            const integer_t y = alea.full<int32_t>();
            const integer   X = x;
            const integer   Y = y;
            const integer   Z = x+y;
            if(Z!=(x+y))
            {
                throw exception("invalid addition");
            }
        }
    }
    
    std::cerr << "-- Subtraction Tests" << std::endl;
    {
        for(integer i=5;i>=-5;--i)
        {
            std::cerr << "i=" << i << std::endl;
        }
        for(size_t i=0;i<10000;++i)
        {
            const integer_t x = alea.full<integer_t>();
            const integer_t y = alea.full<integer_t>();
            const integer   X = x;
            const integer   Y = y;
            const integer   Z = x-y;
            if(Z!=(x-y))
            {
                throw exception("invalid subtraction");
            }
        }
    }

    std::cerr << "-- Multiplication Tests" << std::endl;
    {
        for(size_t i=0;i<10000;++i)
        {
            const integer_t x = alea.full<int32_t>();
            const integer_t y = alea.full<int32_t>();
            const integer   X = x;
            const integer   Y = y;
            const integer   Z = x*y;
            if(Z!=(x*y))
            {
                throw exception("invalid multiplication");
            }
        }

    }
    
    std::cerr << "-- Division Tests" << std::endl;
    {
        for(size_t i=0;i<10000;++i)
        {
            const integer_t x = alea.full<integer_t>();
            const integer_t y = alea.full<integer_t>();
            if(0==y) continue;
            const integer   X = x;
            const integer   Y = y;
            const integer   Z = x/y;
            if(Z!=(x/y))
            {
                throw exception("invalid division");
            }
        }
        
    }

    
    
}
YOCTO_UNIT_TEST_DONE()
