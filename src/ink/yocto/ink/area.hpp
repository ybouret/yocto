#ifndef YOCTO_INK_AREA_INCLUDED
#define YOCTO_INK_AREA_INCLUDED 1

#include "yocto/ink/types.hpp"

namespace yocto
{
    namespace Ink
    {

        class Area : public object
        {
        public:
            const unit_t x;
            const unit_t y;
            const unit_t w;
            const unit_t h;
            const unit_t x_end;
            const unit_t y_end;
            explicit Area( const unit_t X, const unit_t Y, const unit_t W, const unit_t H);
            virtual ~Area() throw();
            Area(const Area  &rect) throw();
            Area(const Patch &p) throw();
            Patch getPatch() const throw();

        private:
            YOCTO_DISABLE_ASSIGN(Area);
        };
        
    }
}

#endif

