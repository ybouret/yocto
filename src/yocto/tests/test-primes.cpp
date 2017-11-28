#include "yocto/utest/run.hpp"
#include "yocto/code/primes.hpp"
#include "yocto/code/alea.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(primes)
{
	size_t p=0;
	for( size_t i=0; i < 25; ++i )
	{
		for( size_t j=0; j < 16; ++j, ++p )
		{
			std::cerr << (p=next_prime(p)) << " ";
		}
		std::cerr << std::endl;
	}
	
	for( size_t i=0; i < 10; ++i )
	{
		for( size_t j=0; j < 4; ++j )
		{
			const size_t n = 2 + alea.lt( 0xFFFFFF );
			std::cerr << "(" << prev_prime(n) << "," << n << "," << next_prime(n) << ") ";
		}
		std::cerr << std::endl;
	}
}
YOCTO_UNIT_TEST_DONE()

YOCTO_UNIT_TEST_IMPL(is_prime)
{
    size_t count = 0;
    for(size_t i=2;;++i)
    {
        if(is_prime(i))
        {
            std::cout << i << std::endl;
            if(++count>10000)
            {
                break;
            }
        }

    }
}
YOCTO_UNIT_TEST_DONE()

    

#include "yocto/code/primes-diff.hpp"
YOCTO_UNIT_TEST_IMPL(Primes)
{
    core::Primes::Print();
    for(size_t i=0;i<=20;++i)
    {
        std::cerr << i << " => " << nextPrime(i) << std::endl;
    }
    
}
YOCTO_UNIT_TEST_DONE()

