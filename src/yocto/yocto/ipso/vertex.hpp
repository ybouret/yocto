#ifndef YOCTO_IPSO_VERTEX_INCLUDED
#define YOCTO_IPSO_VERTEX_INCLUDED 1

#include "yocto/ipso/coord.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename T,const size_t>
        struct vertex_for;

        template <typename T>
        struct vertex_for<T,1>
        {
            typedef T       type;
            template <typename U>
            static inline type from( const U c ) throw()
            {
                return type(c);
            }
        };

        template <typename T>
        struct vertex_for<T,2>
        {
            typedef point2d<T> type;
            template <typename U>
            static inline type from( const point2d<U> c ) throw()
            {
                return type(c.x,c.y);
            }
        };

        template <typename T>
        struct vertex_for<T,3>
        {
            typedef point3d<T> type;
            template <typename U>
            static inline type from( const point3d<U> c ) throw()
            {
                return type(c.x,c.y,c.z);
            }
        };
    }
}

#endif

