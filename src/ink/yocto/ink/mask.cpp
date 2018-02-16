
#include "yocto/ink/mask.hpp"


namespace yocto
{
    namespace Ink
    {
        Mask:: ~Mask() throw()
        {
        }
        
        Mask:: Mask() throw() : MaskType() {}
        Mask:: Mask( const Mask &other ) : MaskType(other) {}
        
    }
}
