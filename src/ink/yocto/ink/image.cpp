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


        Image::Format::Format(const char *id) : ImageIO(id)
        {
        }

        Image::Format:: ~Format() throw()
        {
        }
        
    }
}
