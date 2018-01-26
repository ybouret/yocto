#ifndef YOCTO_INK_IMAGE_TIFF_INCLUDED
#define YOCTO_INK_IMAGE_TIFF_INCLUDED 1

#include "yocto/ink/image.hpp"
#include "yocto/ink/image/tiff++.hpp"

namespace yocto
{
    namespace Ink
    {

        class tiff_format : public Image::Format
        {
        public:
            explicit tiff_format();
            virtual ~tiff_format() throw();
            
            virtual bool     lossless() const throw();
            
            
            //! option input_options...
            virtual Bitmap  *load(const string          &filename,
                                  unit_t                 depth,
                                  rgba2data             &proc,
                                  const void            *options) const;
            
            virtual void     save(const string        &filename,
                                  const Bitmap        &bmp,
                                  data2rgba           &proc,
                                  const void          *options) const;


            size_t count_directories(const string &filename) const;

            //! load a bitmap with 0<=indx<count_directories
            Bitmap *load_bitmap(const string          &filename,
                                unit_t                 depth,
                                rgba2data             &proc,
                                const size_t           indx) const;
            


        private:
            mutable I_TIFF::Raster raster;
            YOCTO_DISABLE_COPY_AND_ASSIGN(tiff_format);
            virtual const char **extensions() const throw();
        };

    }

}

#endif


