#include "yocto/ink/rectangle.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace Ink
    {

        Rectangle:: ~Rectangle() throw()
        {
        }

        Rectangle:: Rectangle( const unit_t X, const unit_t Y, const unit_t W, const unit_t H) throw() :
        x(X),
        y(Y),
        w( max_of<unit_t>(0,W) ),
        h( max_of<unit_t>(0,H) ),
        x_end(x+w-1),
        y_end(y+h-1)
        {
        }

        Rectangle:: Rectangle(const Rectangle &rect) throw() :
        x(rect.x),
        y(rect.y),
        w(rect.w),
        h(rect.h),
        x_end(rect.x_end),
        y_end(rect.y_end)
        {
        }
        

    }

}

