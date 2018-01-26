#ifndef YOCTO_INK_IMAGE_PNG_INCLUDED
#define YOCTO_INK_IMAGE_PNG_INCLUDED 1

#include "yocto/ink/image.hpp"

namespace yocto
{
    namespace Ink
    {
        class png_format : public Image::Format
        {
        public:
            explicit png_format();
            virtual ~png_format() throw();
            
            virtual bool     lossless() const throw();
            virtual Bitmap  *load(const string          &filename,
                                  unit_t                 depth,
                                  rgba2data             &proc,
                                  const void            *options) const;
            
            virtual void     save(const string        &filename,
                                  const Bitmap        &bmp,
                                  data2rgba           &proc,
                                  const void          *options) const;
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(png_format);
            virtual const char **extensions() const throw();
        };
    }
}

#endif

