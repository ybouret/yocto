
#include "yocto/ink/images.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(fmt)
{
    Ink::Image       &IMG = Ink::Image::instance();
    Ink::png_format  &PNG = IMG.create<Ink::png_format>();
    Ink::jpeg_format &JPG = IMG.create<Ink::jpeg_format>();
    Ink::tiff_format &TIF = IMG.create<Ink::tiff_format>();
    (void)PNG;
    (void)JPG;
    (void)TIF;
   // std::cerr << PNG.name << std::endl;
   // std::cerr << JPG.name << std::endl;
   // std::cerr << TIF.name << std::endl;

}
YOCTO_UNIT_TEST_DONE()

