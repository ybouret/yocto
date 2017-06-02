#ifndef YOCTO_FAME_LAYOUTS_INCLUDED
#define YOCTO_FAME_LAYOUTS_INCLUDED 1

#include "yocto/fame/domain.hpp"

namespace yocto
{

    namespace fame
    {
        template <typename COORD>
        class layouts_of : public domain_of<COORD>
        {
        public:
            typedef domain_of<COORD> domain_type;
            typedef layout_of<COORD> layout_type;
            
            const layout_type outer;
            
            inline virtual ~layouts_of() throw() {}
            
            inline layouts_of(const domain_type &dom,
                              const coord1d      num_ghosts) :
            domain_type(dom),
            outer(this->inner)
            {
            }
            
            
        private:
            YOCTO_DISABLE_ASSIGN(layouts_of);
        };
    }

}

#endif
