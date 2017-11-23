#include "yocto/randomized/isaac.hpp"
#include "yocto/utest/run.hpp"
#include <cstdlib>
#include <math.h>
#include <cstdio>

using namespace yocto;
using namespace Randomized;

static inline uint32_t stdRand32()
{
    return uint32_t( floor(0.5 + 4294967296.0 * double( rand() ) / RAND_MAX ) );
}

YOCTO_UNIT_TEST_IMPL(randomized)
{
    srand(time(NULL));
   
    
    ISAAC_FAST::Generator isaac_f(ISAAC_INIT_RAND);
    ISAAC::Generator      isaac(ISAAC_INIT_RAND);
    
    for(int i=0;i<10;++i)
    {
        const uint32_t u = stdRand32();
        const uint8_t  b = Bits::ToByte(u);
        fprintf(stderr,"u32=0x%08x => %02x\n",  unsigned(u),unsigned(b));
        fprintf(stderr,"\tisaac: %08x, %08x\n", unsigned(isaac.next32()),unsigned(isaac_f.next32()));
        fprintf(stderr,"\t full: %08x, %08x\n", isaac.full<unsigned>(), isaac_f.full<unsigned>());
        fprintf(stderr,"\t fuzz: %08x, %08x\n", isaac.fuzz<unsigned>(), isaac_f.fuzz<unsigned>());

    }
    
    
}
YOCTO_UNIT_TEST_DONE()

