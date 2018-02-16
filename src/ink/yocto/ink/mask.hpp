#ifndef YOCTO_INK_MASK_INCLUDED
#define YOCTO_INK_MASK_INCLUDED 1

#include "yocto/associative/hlist.hpp"
#include "yocto/ink/types.hpp"

namespace yocto
{
    namespace Ink
    {
        
        typedef hlist<coord,Vertex> MaskType;
        
        class Mask : public MaskType
        {
        public:
            explicit Mask() throw();
            virtual ~Mask() throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Mask);
        };
    }
}

#endif

