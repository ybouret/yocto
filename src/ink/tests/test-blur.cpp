#include "yocto/ink/ops/blur.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace Ink;


YOCTO_UNIT_TEST_IMPL(blur)
{
    {
        ios::wcstream fp("blur.dat");
        for(float sig=0.01f;sig<=10;sig+=0.01f)
        {
            const Blur blur(sig);
            fp( "%g %u %g\n", blur.sigma, unsigned(blur.length), blur(blur.length)  );
        }
    }
}
YOCTO_UNIT_TEST_DONE()

