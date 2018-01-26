#ifndef YOCTO_INK_RECTANGLE_INCLUDED
#define YOCTO_INK_RECTANGLE_INCLUDED 1

#include "yocto/ink/types.hpp"

namespace yocto
{
    namespace Ink
    {

        class Rectangle
        {
        public:
            const unit_t x;
            const unit_t y;
            const unit_t w;
            const unit_t h;
            const unit_t x_end;
            const unit_t y_end;
            Rectangle( const unit_t X, const unit_t Y, const unit_t W, const unit_t H) throw();
            Rectangle(const Rectangle &rect) throw();
            ~Rectangle() throw();

        private:
            YOCTO_DISABLE_ASSIGN(Rectangle);
        };

    }
}

#endif

