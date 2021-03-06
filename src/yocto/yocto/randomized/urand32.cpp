#include "yocto/randomized/urand32.hpp"
#include <cstdio>


namespace yocto
{
    namespace Randomized
    {
#define znew   (z=36969*(z&65535)+(z>>16))
#define wnew   (w=18000*(w&65535)+(w>>16))
#define MWC    ((znew<<16)+wnew )
#define SHR3  (jsr^=(jsr<<17), jsr^=(jsr>>13), jsr^=(jsr<<5))
#define CONG  (jcong=69069*jcong+1234567)
#define FIB   ((b=a+b),(a=b-a))
#define KISS  ((MWC^CONG)+SHR3)
#define LFIB4 (c++,t[c]=t[c]+t[UC(c+58)]+t[UC(c+119)]+t[UC(c+178)])
#define SWB   (c++,bro=(x<y),t[c]=(x=t[UC(c+34)])-(y=t[UC(c+19)]+bro))
#define UNI   (KISS*2.328306e-10)
#define VNI   ((long) KISS)*4.656613e-10
#define UC    (uint8_t)  /*a cast operation*/
        
        void __rand32::test(void)
        {
            fflush(stdout);
            __rand32 r;
            r.settable(12345,65435,34221,12345,9983651,95746118);
            uint32_t k;
            int      i;
            printf("==> rand32 tests\n");
            for(i=1;i<1000001;i++){k=r.lfib4();} printf("%u\n", k-1064612766U);
            for(i=1;i<1000001;i++){k=r.swb()  ;} printf("%u\n", k- 627749721U);
            for(i=1;i<1000001;i++){k=r.kiss() ;} printf("%u\n", k-1372460312U);
            for(i=1;i<1000001;i++){k=r.cong() ;} printf("%u\n", k-1529210297U);
            for(i=1;i<1000001;i++){k=r.shr3() ;} printf("%u\n", k-2642725982U);
            for(i=1;i<1000001;i++){k=r.mwc()  ;} printf("%u\n", k- 904977562U);
            for(i=1;i<1000001;i++){k=r.fib()  ;} printf("%u\n", k-3519793928U);
            printf("==> done\n");
        }
        
        
#if defined(_MSC_VER)
        // init of t
#pragma warning ( disable : 4351 )
#endif
        __rand32:: __rand32() throw() :
        z(362436069),
        w(521288629),
        jsr(123456789),
        jcong(380116160),
        a(224466889),
        b(7584631),
        x(0),
        y(0),
        bro(0),
        c(0),
        t()
        {
            for(size_t i=0;i<256;++i)
                t[i]=KISS;
        }
        
        void __rand32:: settable( uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4, uint32_t i5, uint32_t i6 ) throw()
        {
            z=i1;
            w=i2,
            jsr=i3;
            jcong=i4;
            a=i5;
            b=i6;
            for(size_t i=0;i<256;++i)
                t[i]=KISS;
            c=0;
        }
        
        static inline
        uint32_t __ih32( uint32_t a)
        {
            a = (a+0x7ed55d16) + (a<<12);
            a = (a^0xc761c23c) ^ (a>>19);
            a = (a+0x165667b1) + (a<<5);
            a = (a+0xd3a2646c) ^ (a<<9);
            a = (a+0xfd7046c5) + (a<<3);
            a = (a^0xb55a4f09) ^ (a>>16);
            return a;
        }
        
        void __rand32:: reset( uint32_t s ) throw()
        {
            uint32_t iv[6];
            iv[0] = __ih32(s);
            for( size_t i=1; i < 6; ++i )
                iv[i] = __ih32( iv[i-1] );
            settable(iv[0], iv[1], iv[2], iv[3], iv[4], iv[5]);
        }
        
        
    }
    
}




namespace yocto
{
    namespace Randomized
    {
        
        __rand32:: ~__rand32() throw()
        {
        }
        
        uint32_t __rand32:: mwc() throw()
        {
            return MWC;
        }
        
        uint32_t __rand32:: shr3() throw()
        {
            return SHR3;
        }
        
        uint32_t __rand32:: cong() throw()
        {
            return CONG;
        }
        
        uint32_t __rand32:: fib() throw()
        {
            return FIB;
        }
        
        uint32_t __rand32:: kiss() throw()
        {
            return KISS;
        }
        
        uint32_t __rand32:: lfib4() throw()
        {
            return LFIB4;
        }
        
        uint32_t __rand32:: swb() throw()
        {
            return SWB;
        }
    }
}

#include "yocto/sys/wtime.hpp"
#include "yocto/sys/pid.hpp"
#include "yocto/hashing/hash64.hpp"

namespace yocto
{
    namespace Randomized
    {
        static
        inline uint64_t __h64( const uint64_t x ) throw()
        {
            return hashing::hash64::mix64(x, hashing::hash64::NR);
        }
        
        void __rand32:: initialize() throw()
        {
            const uint64_t w0 = __h64(wtime::ticks()+ get_process_h32());
            const uint64_t w1 = __h64(w0+wtime::ticks());
            const uint64_t w2 = __h64(w1+wtime::ticks());
            union
            {
                uint64_t qw[3];
                uint32_t iv[6];
            } u = { {w0,w1,w2} };
            settable(u.iv[0],
                     u.iv[1],
                     u.iv[2],
                     u.iv[3],
                     u.iv[4],
                     u.iv[5]);
#if 0
            fprintf(stderr,"64: %016llx %016llx %016llx\n",u.qw[0],u.qw[1],u.qw[2]);
            fprintf(stderr,"32: %08x %08x %08x %08x %08x %08x\n",
                    u.iv[0],u.iv[1],u.iv[2],u.iv[3],u.iv[4],u.iv[5]);
#endif
        }
    }
}
