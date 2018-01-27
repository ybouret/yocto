#include "yocto/ink/image.hpp"

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
        

        Image::Format::Format(const char *id) : ImageIO(id)
        {
        }

        Image::Format:: ~Format() throw()
        {
        }
        
        Image:: ~Image() throw()
        {
        }
        
        Image:: Image() : formats(4,as_capacity)
        {
        }
        
        
        void Image:: enroll( const Format::Pointer &format)
        {
            std::cerr << format->name << std::endl;
            if(!formats.insert(format))
            {
                throw exception("Image: multiple '%s'", *(format->name) );
            }
           
           
        }

    }
}
