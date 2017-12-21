#include "yocto/code/primality.hpp"
//#include <iostream>

namespace yocto
{

    bool primality:: check0( const size_t n ) throw()
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


    bool primality:: check1(const size_t n) throw()
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
                if( (!(n%2)) || (!(n%3)) )
                    return false;
                else
                {
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
        }
    }


#include "prmcodes.inc"


    bool primality:: check2(const size_t n) throw()
    {
        switch(n)
        {
            case 0:
            case 1:
                return false;
            case 2:
            case 3:
                return true;

            default:
                break;
        }

        if(0==(n%2))
        {
            return false;
        }
        else
        {
            size_t j=0;
            size_t p=3;
            do
            {
                if(!(n%p))
                    return false;

                if(j>=ncode)
                {
                    break;
                }

                p += ( (1+codes[j++])<<1 );
                
            } while(p*p<=n);

            for(size_t i=icode;i*i<=n;i+=6)
            {
                if( (!(n%i)) || (!(n%(i+2))) )
                {
                    return false;
                }
            }
            return true;
        }
    }



}
