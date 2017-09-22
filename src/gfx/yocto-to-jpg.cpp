
#include "yocto/gfx/image.hpp"
#include "yocto/gfx/image/jpeg.hpp"
#include "yocto/gfx/image/png.hpp"
#include "yocto/gfx/image/tiff.hpp"

#include "yocto/string/conv.hpp"

#include "yocto/program.hpp"

using namespace yocto;
using namespace gfx;

YOCTO_PROGRAM_START()
{

    const image::format &JPG = YOCTO_GFX_DECL_FORMAT(jpeg);
    YOCTO_GFX_DECL_FORMAT(png);
    YOCTO_GFX_DECL_FORMAT(tiff);

    imageIO          &IMG = image::instance();

    if(argc>1)
    {
        const string  inputName = argv[1];
        const pixmap3 img0( IMG.load3(inputName, NULL) );
        JPG.save("tmp.jpg", img0, "quality=50");
    }

}
YOCTO_PROGRAM_END()

