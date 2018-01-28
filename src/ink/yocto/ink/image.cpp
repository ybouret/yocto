#include "yocto/ink/image.hpp"
#include "yocto/fs/vfs.hpp"

namespace yocto
{

    namespace Ink
    {
        ImageIO:: ~ImageIO() throw()
        {

        }

        ImageIO:: ImageIO(const char *id) :
        name(id)
        {
        }

        const string & ImageIO:: key() const throw() { return name; }

        Bitmap * ImageIO:: loadRGBA(const string &filename, const void *options) const
        {
            PutRGBA proc;
            return load(filename,4,proc,options);
        }

        Bitmap * ImageIO:: loadRGB(const string &filename, const void *options) const
        {
            PutRGB proc;
            return load(filename,3,proc,options);
        }

        Bitmap * ImageIO:: loadGSF(const string &filename, const void *options) const
        {
            PutGSF proc;
            return load(filename,sizeof(float),proc,options);
        }

        Bitmap * ImageIO:: loadGSU(const string &filename, const void *options) const
        {
            PutGSU proc;
            return load(filename,sizeof(uint8_t),proc,options);
        }

        Bitmap * ImageIO:: loadYUV(const string &filename, const void *options) const
        {
            PutYUV proc;
            return load(filename,sizeof(YUV),proc,options);
        }

    }

}



namespace yocto
{

    namespace Ink
    {

        Image::Format::Format(const char *id) : ImageIO(id)
        {
        }

        Image::Format:: ~Format() throw()
        {
        }

        const char Image::name[] = "Image";


        Image:: ~Image() throw()
        {
        }
        
        Image:: Image() :
        ImageIO(name),
        formats(16,as_capacity),
        extfmts(16,as_capacity)
        {
        }
        
        
        void Image:: enroll( const Format::Pointer &format)
        {
            if(!formats.insert(format))
            {
                throw exception("%s: multiple '%s'", name, *(format->name) );
            }

            for(const char **pExt = format->extensions(); *pExt; ++pExt )
            {
                string ext( *pExt );
                ext.to_lower();
                if(!extfmts.insert(ext,format))
                {
                    throw exception("%s: multiple extension '%s' from format '%s'", name, *ext, *(format->name) );
                }
            }

        }

        Bitmap  *Image::load(const string          &filename,
                             unit_t                 depth,
                             rgba2data             &proc,
                             const void            *options) const
        {
            
            return get_format_for(filename)->load(filename,depth,proc,options);
        }


        void Image::  save(const string        &filename,
                           const Bitmap        &bmp,
                           data2rgba           &proc,
                           const void          *options) const
        {
            get_format_for(filename)->save(filename,bmp,proc,options);
        }


        const Image::Format * Image:: get_format_for( const string &filename) const
        {
            string ext = vfs::get_extension(filename);
            ext.to_lower();
            const Format::Pointer *ppFmt = extfmts.search(ext);
            if(!ppFmt)
            {
                throw exception("%s: no format to handle '%s'", name, *ext);
            }
            return & (**ppFmt);
        }

        void Image:: display() const
        {
            std::cerr << name << " extensions/formats" << std::endl;
            for( Format::Register::const_iterator i=extfmts.begin();i!=extfmts.end();++i)
            {
                const string          &ext = i->key;
                const Format::Pointer &fmt = *i;
                std::cerr << "extension '" << ext << "'@format='" << fmt->name << std::endl;
            }
        }

    }
}





#include "yocto/ink/image/jpeg.hpp"
#include "yocto/ink/image/png.hpp"
#include "yocto/ink/image/tiff.hpp"

namespace yocto
{
    namespace Ink
    {
        void Image:: Init()
        {
            (void) create<png_format>();
            (void) create<jpeg_format>();
            (void) create<tiff_format>();
        }
    }
}
