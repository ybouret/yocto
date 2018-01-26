#ifndef YOCTO_INK_IMAGE_JPEG_INCLUDED
#define YOCTO_INK_IMAGE_JPEG_INCLUDED 1

#include "yocto/ink/image.hpp"

namespace yocto
{
    namespace Ink
    {
        class jpeg_format : public Image::Format
        {
        public:
            explicit jpeg_format();
            virtual ~jpeg_format() throw();
            
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
            YOCTO_DISABLE_COPY_AND_ASSIGN(jpeg_format);
            virtual const char **extensions() const throw();
        };
    }
}

#endif

