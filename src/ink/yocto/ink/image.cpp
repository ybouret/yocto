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
            std::cerr << format->name << std::endl;
            if(!formats.insert(format))
            {
                throw exception("%s: multiple '%s'", name, *(format->name) );
            }

            for(const char **pExt = format->extensions(); *pExt; ++pExt )
            {
                string ext( *pExt );
                ext.to_lower();
                std::cerr << "taking " << ext << std::endl;
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

    }
}
