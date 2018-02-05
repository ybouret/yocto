#include "yocto/ink/area.hpp"
#include "yocto/exceptions.hpp"
#include <cerrno>

namespace yocto
{
    namespace Ink
    {

        Area:: ~Area() throw()
        {
        }

        static inline
        unit_t __check(const char *name, const unit_t value)
        {
            assert(name);
            if(value<=0) throw libc::exception(ERANGE,"invalid %s=%d", name, int(value));
            assert(value>0);
            return value;
        }

        Area:: Area( const unit_t X, const unit_t Y, const unit_t W, const unit_t H)  :
        x(X),
        y(Y),
        w( __check("width", W) ),
        h( __check("height",H) ),
        x_end(x+w-1),
        y_end(y+h-1)
        {
        }

        Area:: Area(const Area &rect) throw() :
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

