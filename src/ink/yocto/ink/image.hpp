#ifndef YOCTO_INK_IMAGE_INCLUDED
#define YOCTO_INK_IMAGE_INCLUDED 1

#include "yocto/ink/bitmap.hpp"
#include "yocto/ink/color/rgba2data.hpp"
#include "yocto/ink/color/data2rgba.hpp"
#include "yocto/ptr/intr.hpp"
#include "yocto/associative/set.hpp"

namespace yocto
{
    namespace Ink
    {

        //! Interface to an Image Input/Output
        class ImageIO : public counted_object
        {
        public:
            virtual ~ImageIO() throw();
            const string name;
            const string & key() const throw();
            
            //==============================================================
            //
            // virtual interface
            //
            //==============================================================
            virtual Bitmap  *load(const string          &filename,
                                  unit_t                 depth,
                                  rgba2data             &proc,
                                  const void            *options) const = 0;

            virtual void     save(const string        &filename,
                                  const Bitmap        &bmp,
                                  data2rgba           &proc,
                                  const void          *options) const = 0;

        protected:
            ImageIO(const char *id);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ImageIO);
        };

        
        
        class Image
        {
        public:
            explicit Image();
            virtual ~Image() throw();
            class Format : public ImageIO
            {
            public:
                typedef intr_ptr<string,Format> Pointer;
                typedef set<string,Pointer>     DataBase;
                
                virtual ~Format() throw();

                virtual bool         lossless() const throw() = 0;
                virtual const char **extensions() const throw() = 0;

            protected:
                explicit Format(const char *id);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Format);
            };
            
            template <typename  FMT>
            FMT & create()
            {
                FMT *fmt = new FMT();
                const Format::Pointer format(fmt);
                enroll(fmt);
                return  *fmt;
            }
            
            void enroll( const Format::Pointer &format);
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Image);
            Format::DataBase formats;            
        };

    }
}

#endif
