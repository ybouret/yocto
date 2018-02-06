#include "yocto/ink/image.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/sys/wtime.hpp"

#include "yocto/ink/color/ramp/cold_to_hot.hpp"
#include "yocto/ink/color/ramp/cold_to_very_hot.hpp"
#include "yocto/ink/color/ramp/grey.hpp"
#include "yocto/ink/color/ramp/orange.hpp"
#include "yocto/ink/color/ramp/blue_to_red.hpp"


using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(ramp)
{
    YOCTO_IMG();

    ramp_cold_to_hot      r1;
    ramp_cold_to_very_hot r2;
    ramp_grey             r3(0,1);
    ramp_orange           r4;
    ramp_blue_to_red      r5;


    if(argc>1)
    {
        const Pixmap<float> pxm3( IMG.loadGSF(argv[1],NULL) );
        IMG.save("img_r1.jpg", pxm3, r1, NULL);
        IMG.save("img_r2.jpg", pxm3, r2, NULL);
        IMG.save("img_r3.jpg", pxm3, r3, NULL);
        IMG.save("img_r4.jpg", pxm3, r4, NULL);
        IMG.save("img_r5.jpg", pxm3, r5, NULL);

    }

}
YOCTO_UNIT_TEST_DONE()

