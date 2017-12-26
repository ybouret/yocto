#include "yocto/mpl/natural.hpp"
//#include "yocto/code/primes16.hpp"

namespace yocto
{
    namespace mpl
    {



        bool natural:: __is_prime( const natural &n )
        {

            if(n<=1)
            {
                return false;
            }
            else
            {
                if(n<=3)
                {
                    return true;
                }
                else
                {
                    assert(n>=4);
                    if( n.is_divisible_by(2) || n.is_divisible_by(3) )
                    {
                        return false;
                    }
                    else
                    {
                        assert(n>=5);
                        const mpn two(2);
                        const mpn six(6);
                        mpn i = 5;
                        while(true)
                        {
                            const mpn i2 = sqr(i);
                            if(i2>n) return true;
                            if( n.is_divisible_by(i) )
                            {
                                return false;
                            }
                            else
                            {
                                const mpn j = i+two;
                                if(n.is_divisible_by(j))
                                {
                                    return false;
                                }
                            }

                            i += six;
                        }

                    }
                }
            }
        }

        natural natural:: __next_prime(const natural &n )
        {
            if( n <= 2 )
            {
                return 2;
            }
            else
            {
                const natural two(2);
                mpn q = n;
                if( q.is_even() ) ++q;
                while( ! __is_prime(q) )
                {
                    q += two;
                }
                return q;
            }

        }

    }

}
