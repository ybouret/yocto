#include "yocto/ink/types.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(types)
{
    for(unsigned r=0;r<256;++r)
    {
        for(unsigned g=0;g<256;++g)
        {
            for(unsigned b=0;b<256;++b)
            {
                const float f = Ink::Core::uFloatR[r] + Ink::Core::uFloatG[g] + Ink::Core::uFloatB[b];
                if(f<0.0f) throw exception("%u.%u.%u grey<0.0",r,g,b);
                if(f>1.0f) throw exception("%u.%u.%u grey>1.0",r,g,b);
            }
        }
    }
}

YOCTO_UNIT_TEST_DONE()

