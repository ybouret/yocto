#ifndef YOCTO_INK_IMAGE_INCLUDED
#define YOCTO_INK_IMAGE_INCLUDED 1

#include "yocto/ink/pixmaps.hpp"
#include "yocto/ink/color/rgba2data.hpp"
#include "yocto/ink/color/data2rgba.hpp"
#include "yocto/ptr/intr.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/associative/map.hpp"
#include "yocto/threading/singleton.hpp"

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

            //==============================================================
            //
            // Non-virtual interface
            //
            //==============================================================
            Bitmap *loadRGBA(const string &filename, const void *options) const;
            Bitmap *loadRGB( const string &filename, const void *options) const;
            Bitmap *loadGSF( const string &filename, const void *options) const;
            Bitmap *loadGSU( const string &filename, const void *options) const;
            Bitmap *loadYUV( const string &filename, const void *options) const;

            void    save(const PixmapRGBA &pxm, const string &filename, const void *options) const;
            void    save(const PixmapRGB  &pxm, const string &filename, const void *options) const;
            void    save(const PixmapF    &pxm, const string &filename, const void *options) const;
            void    save(const PixmapU    &pxm, const string &filename, const void *options) const;
            void    save(const PixmapYUV  &pxm, const string &filename, const void *options) const;

        protected:
            ImageIO(const char *id);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ImageIO);
        };

        
        //! interface to dispatch I/O accorging to registered formats and extensions
        class Image : public ImageIO, public singleton<Image>
        {
        public:
            virtual ~Image() throw();

            //! extends ImageIO to be handled by Image
            class Format : public ImageIO
            {
            public:
                typedef intr_ptr<string,Format> Pointer;
                typedef set<string,Pointer>     DataBase;
                typedef map<string,Pointer>     Register;

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

            virtual Bitmap  *load(const string          &filename,
                                  unit_t                 depth,
                                  rgba2data             &proc,
                                  const void            *options) const;

            virtual void     save(const string        &filename,
                                  const Bitmap        &bmp,
                                  data2rgba           &proc,
                                  const void          *options) const;

            void Init(); //!< initialize with JPG/PNG/TIF
            void display() const;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Image);
            explicit Image();

            friend class singleton<Image>;
            static const char                  name[];
            static const threading::longevity life_time = 0;

            Format::DataBase formats;
            Format::Register extfmts;
            const Format * get_format_for( const string &filename) const;
        };

#define YOCTO_IMG() \
yocto::Ink::Image &IMAGES = yocto::Ink::Image::instance(); \
IMAGES.Init(); \
const yocto::Ink::ImageIO &IMG  = IMAGES

#define YOCTO_IMG_IO() const yocto::Ink::ImageIO &IMG = yocto::Ink::Image::instance()
    }
}

#endif

