#ifndef YOCTO_GFX_IMAGE_JPEG_INCLUDED
#define YOCTO_GFX_IMAGE_JPEG_INCLUDED 1

#include "yocto/gfx/image.hpp"

namespace yocto
{
    namespace gfx
    {
        class jpeg_format : public image::format
        {
        public:
            explicit jpeg_format();
            virtual ~jpeg_format() throw();
            
            virtual bool     lossless() const throw();
            virtual bitmap  *load(const string          &filename,
                                  unit_t                 depth,
                                  rgba2data             &proc,
                                  const void            *options) const;
            
            virtual void     save(const string        &filename,
                                  const bitmap        &bmp,
                                  data2rgba           &proc,
                                  const void          *options) const;
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(jpeg_format);
            virtual const char **extensions() const throw();
        };
    }
}

#endif

