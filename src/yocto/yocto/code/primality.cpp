#include "yocto/code/primality.hpp"
#include "yocto/exceptions.hpp"
#include <cerrno>

namespace yocto
{

    bool primality:: _check( const size_t n ) throw()
    {
        switch(n)
        {
            case 0:
            case 1:
                return false;
            case 2:
                return true;
            default:
                break;
        }

        if(!(n%2)) return false;
        for(size_t i=3;i*i<=n;i+=2)
        {
            if(!(n%i)) return false;
        }
        return true;
    }


    bool primality:: check(const size_t n) throw()
    {

        if(n<=1)
            return false; //!< for 0 and 1
        else if(n<=3)
            return true;  //!< for 2 and 3
        else if( !(n%2) || !(n%3) ) //! for 2,4,6,8,9...
            return false;
        else
        {
            assert(n>=5);
            for(size_t i=5;i*i<=n;i+=6)
            {
                if( (!(n%i)) || (!(n%(i+2))) )
                {
                    return false;
                }
            }
            return true;
        }
    }

#if 0
    bool primality:: check2(const size_t n) throw()
    {
#include "/Users/ybouret/program/yocto/prime32.cxx"
    }
#endif
    
    size_t primality:: prev(const size_t n)
    {
        switch(n)
        {
            case 0:
            case 1:
                throw libc::exception( EDOM, "no previous prime for %u", unsigned(n) );
            case 2:
                return 2;
            default:
                break;
        }
        assert(n>=3);
        size_t p = n;
        while( 0 == (p&1) ) --p;
        while( !check(p) ) p -= 2;
        return p;
    }

    size_t primality:: next(const size_t n)
    {
        if(n<=2)
        {
            return 2;
        }
        else
        {
            size_t p = n|1;
            while( !check(p) ) p+=2;
            return p;
        }

    }



}
