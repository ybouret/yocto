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
            Mask(const Mask &other);
            virtual ~Mask() throw();
            
            void add(const coord p)
            {
                if(!search(p))
                {
                    if(!insert(p, new Vertex(p) ))
                        throw exception("unexpected mask add failure");
                }
            }
            
        private:
            YOCTO_DISABLE_ASSIGN(Mask);
        };
    }
}

#endif

