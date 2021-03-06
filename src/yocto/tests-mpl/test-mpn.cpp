#include "yocto/mpl/natural.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/alea.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/sort/quick.hpp"
#include <cmath>

using namespace yocto;
using namespace mpl;

#define _SHOW(X) std::cerr << #X " = " << X << std::endl

YOCTO_UNIT_TEST_IMPL(mpn)
{
    std::cerr << "-- Basic Tests" << std::endl;
    {
        mpn n0; _SHOW(n0);
        mpn n1 = 0x12;     _SHOW(n1);
        mpn n2 = 0x0234;   _SHOW(n2);
        mpn n3 = 0x123456; _SHOW(n3);

        for(size_t i=0;i<1000;++i)
        {
            word_t x = alea.full<word_t>();
            mpn    X = x;
            if(X.lsw()!=x) throw exception("invalid lsw!");
            mpn    Y(X);
            if(Y.lsw()!=x) throw exception("invalid lsw level-2");
            if(X!=Y)       throw exception("invalid copy, level-1");
            if(!(X==Y))    throw exception("invlaid copy, level-2");
            Y.ldz();
            Y = X;
            if(X!=Y)       throw exception("invalid assign");
        }
    }

    std::cerr << "-- Comparison Tests" << std::endl;
    {
        vector<mpn> values;

        for(size_t i=0;i<1000;++i)
        {
            word_t x = alea.full<word_t>();
            word_t y = alea.full<word_t>();
            mpn    X = x;
            mpn    Y = y;
            const int c = __compare(x,y);
            const int C = __compare(X,Y);
            if( c != C )
            {
                std::cerr << std::hex;
                std::cerr << "x=" << x << "  | X=" << X << std::endl;
                std::cerr << "y=" << y << "  | Y=" << Y << std::endl;
                std::cerr << std::dec;
                std::cerr << "c=" << c << "  | C=" << C << std::endl;
                throw exception("invalid comparison");
            }
            mpn tmp  = alea.full<word_t>();
            values.push_back(tmp);
        }
        quicksort(values);
        for(size_t i=1;i<values.size();++i)
        {
            if(values[i]>values[i+1])
                throw exception("sort failure");
        }
    }

    std::cerr << "-- I/O tests" << std::endl;
    {

        ios::bitio     Q;
        vector<mpn>    values;
        vector<size_t> vbits;
        size_t count = 0;
        for(size_t i=0;i<100;++i)
        {
            const size_t n = alea.leq(100);
            const mpn    v = mpn::rand(n);
            values.push_back(v);
            vbits.push_back(n);
            count += n;
            v.put(Q);
        }
        if(count!=Q.size()) throw exception("Enqueue bits failure");

        for(size_t i=1;i<=values.size();++i)
        {
            const mpn n = mpn::get(Q,vbits[i]);
            if( n != values[i] )
            {
                throw exception("Invalid I/O!");
            }
        }

        values.free();
        Q.free();
        for(size_t i=0;i<100;++i)
        {
            const size_t n = alea.leq(200);
            const mpn    v = mpn::rand(n);
            values.push_back(v);
            v.put(Q,200);
        }
        for(size_t i=1;i<=values.size();++i)
        {
            const mpn n = mpn::get(Q,200);
            if(n!=values[i])
            {
                throw exception("Invalid I/O level-2");
            }
        }
    }


    std::cerr << "-- Addition Tests" << std::endl;
    {
        vector<mpn> values;
        for(size_t i=0;i<1000;++i)
        {
            word_t x = alea.full<uint32_t>();
            word_t y = alea.full<uint32_t>();
            word_t z = x + y;

            mpn X = x;
            mpn Y = y;
            mpn Z = X+Y;

            if(Z!=z) throw exception("invalid addition");

            const mpn tmp = alea.full<word_t>();
            values.push_back(tmp);
        }
        mpn s1 = 0;
        for(size_t i=values.size();i>0;--i)
        {
            s1 += values[i];
        }
        alea.shuffle(values(),values.size());
        mpn s2 = 0;
        for(size_t i=values.size();i>0;--i)
        {
            s2 += values[i];
        }
        std::cerr << "s1=" << s1 << ", s2=" << s2 << std::endl;
        if(s1!=s2) throw exception("invalid addition");
    }

    std::cerr << "-- Subtraction Tests" << std::endl;
    {
        for(size_t i=0;i<1000;++i)
        {
            word_t x = alea.full<word_t>();
            word_t y = alea.full<word_t>();
            if(x<y) cswap(x,y);
            word_t z = x-y;
            mpn X = x;
            mpn Y = y;
            mpn Z = X-Y;
            if( Z != z )
                throw exception("Subtraction failure");
        }
    }

    std::cerr << "-- Multiplication Tests" << std::endl;
    {
        for(size_t i=0;i<1000;++i)
        {
            word_t x = alea.full<uint32_t>();
            word_t y = alea.full<uint32_t>();
            word_t z = x*y;
            mpn X = x;
            mpn Y = y;
            mpn Z = X*Y;
            if( Z != z )
                throw exception("Multiplication failure");
            if( X*X != mpn::sqr(X) )
                throw exception("Square Failure");
        }
    }

    std::cerr << "-- Division/Modulo Tests" << std::endl;
    {
        const size_t nv = 200;
        vector<mpn> values(nv,as_capacity);
        for(size_t i=0;i<nv;++i)
        {
            const mpn tmp = mpn::rand(1+alea.leq(500));
            values.push_back(tmp);
        }
        for(size_t i=1;i<=nv;++i)
        {
            const mpn &num = values[i];
            for(size_t j=1;j<=nv;++j)
            {
                const mpn &den = values[i];
                const mpn    q = num/den;
                const mpn   qd = q * den;
                if(q*den>num) throw exception("invalid division!!!");
                if(i==j)
                {
                    if( ! q.is_byte(1) ) throw exception("invalid auto-division!");
                }
                const mpn   r = num%den;
                if(q*den+r!=num)
                    throw exception("invalid modulo");
                mpn qq;
                mpn rr;
                mpn::split(qq, rr, num, den);
                if(qq!=q) throw exception("split quotient failure!");
                if(rr!=r) throw exception("split remain   failure!");
            }
        }
    }

    std::cerr << "-- Pascal Triangle" << std::endl;
    for(word_t n=1;n<=10;++n)
    {
        for(word_t k=0;k<=n+1;++k)
        {
            std::cerr << mpn::comb(n,k) << ' ';
        }
        std::cerr << std::endl;
    }

    std::cerr << "-- Testing Arithmetic" << std::endl;
    {
        mpn b = 13;
        mpn e = 7;
        mpn N = 9;
        mpn x = natural::mod_exp(b,e,N);
        mpn y = natural::power(b,e) % N;
        std::cerr << "x=" << x << std::endl;
        std::cerr << "y=" << y << std::endl;
        size_t n = 0;
        mpn    p = 0;
        while( n<100 )
        {
            p = natural::__next_prime(++p);
            std::cerr << p << ' ';
            ++n;
        }
        std::cerr << std::endl;
    }




    std::cerr << "-- Testing RSA" << std::endl;
    std::cerr << std::hex;
    for(size_t iter=1;iter<=10;++iter)
    {
        mpn p = mpn::rand(6+alea.leq(25));
        mpn q = mpn::rand(6+alea.leq(25));
        p = mpn::__next_prime(p);
        std::cerr << "\tp=" << p << std::endl;
        q = mpn::__next_prime(q);
        std::cerr << "\tq=" << q << std::endl;
        const mpn n   = p*q;
        const mpn phi = (p-1)*(q-1);
        std::cerr << "\tphi=" << phi << std::endl;
        mpn e = mpn::rand(5);
        while( ! mpn::are_coprime(e, phi) ) ++e;
        std::cerr << "\te=" << e << std::endl;
        const mpn g = mpn::gcd(e,phi);
        std::cerr << "\tg=" << g << std::endl;
        mpn d = mpn::mod_inv(e,phi);
        std::cerr << "\td=" << d << std::endl;
        const size_t bmax = n.bits()-2;
        for(size_t i=1;i<=10;++i)
        {
            const mpn M = mpn::rand(alea.leq(bmax));
            const mpn C = mpn::mod_exp(M,e,n);
            const mpn P = mpn::mod_exp(C,d,n);
            std::cerr << "M=" << M << " => C=" << C << "=> P=" << P << std::endl;
            if(P!=M) throw exception("invalid arithmetic!");
        }

    }
    std::cerr << std::dec;

    std::cerr << "-- Testing conversion" << std::endl;
    {
        for(size_t p=0;p<=10;++p)
        {
            const double pp    = (1<<p);
            const double invpp = 1.0/pp;
            double ip=0;
            double nd=0;
            double q=1;
            for(;;)
            {
                const double tmp = q*invpp;
                if( modf(tmp,&ip) > 0 )
                {
                    ++nd;
                    q*=10;
                }
                else
                    break;
            }
            std::cerr << "2^" << p << "=" << pp << "-> " << invpp << " -> #decimal=" << nd << std::endl;
        }
        for(size_t i=1;i<=10;++i)
        {
            mpn n = mpn::rand(alea.leq(40));
            double x = n.to_real();
            std::cerr << n << " -> " << x << std::endl;
        }
        for(size_t i=1;i<=10;++i)
        {
            const mpn num = mpn::rand(alea.leq(40));
            const mpn den = mpn::rand(alea.leq(40));
            if(den>0)
            {
                const double x = num.to_real();
                const double y = den.to_real();
                const double z = mpn::ratio_of(num,den);
                std::cerr << num << "/" << den << "->" << x/y << " | " << z << std::endl;
            }
        }
    }


}
YOCTO_UNIT_TEST_DONE()

#include "yocto/ios/icstream.hpp"

YOCTO_UNIT_TEST_IMPL(parse)
{
    ios::icstream fp( ios::cstdin );
    string line;
    while( line.clear(), (std::cerr << "> ").flush(), fp.read_line(line) >= 0 )
    {
        const mpn n = mpn::parse(line);
        std::cerr << "\t 0x" << std::hex << n << " | " << std::dec << n << std::endl;
    }

}
YOCTO_UNIT_TEST_DONE()

