#ifndef YOCTO_POINT2D_INCLUDED
#define YOCTO_POINT2D_INCLUDED 1

#include "yocto/bitwise.hpp"
#include "yocto/type/args.hpp"
#include "yocto/math/types.hpp"
#include "yocto/code/rand32.hpp"
#include <iostream>

namespace yocto
{
    YOCTO_MATH_DECL(two_pi);
    
    template <typename T>
    class point2d
    {
    public:
        YOCTO_ARGUMENTS_DECL_T;

        type x;
        type y;

        //______________________________________________________________________
        //
        // POD behavior
        //______________________________________________________________________
        inline  point2d() : x(0), y(0) {}
        inline  point2d( param_type X, param_type Y ) : x(X), y(Y) {}
        inline ~point2d() throw() {}
        inline  point2d(const point2d &other) : x(other.x), y(other.y) {}
        inline  point2d(const point2d a, const point2d b) throw() : x(b.x-a.x), y(b.y-a.y) {}
        inline  point2d & operator=(const point2d &other)
        {
            if(this!=&other)
            {
                x = other.x;
                y = other.y;
            }
            return *this;
        }

        inline friend std::ostream & operator<<( std::ostream &os, const point2d &p )
        {
            os << '[' << p.x << ' ' << p.y << ']' << '\'';
            return os;
        }

        //______________________________________________________________________
        //
        // addition
        //______________________________________________________________________
        inline friend point2d   operator+ (const point2d &p) { return p; }
        inline friend point2d   operator+ (const point2d &lhs,const point2d &rhs) { return point2d(lhs.x+rhs.x,lhs.y+rhs.y); }
        inline        point2d & operator+=(const point2d &rhs) { x+=rhs.x; y+=rhs.y; return *this;}

        //______________________________________________________________________
        //
        // subtraction
        //______________________________________________________________________
        inline friend point2d   operator- (const point2d &p) { return point2d(-p.x,-p.y); }
        inline friend point2d   operator- (const point2d &lhs,const point2d &rhs) { return point2d(lhs.x-rhs.x,lhs.y-rhs.y); }
        inline        point2d & operator-=( const point2d &rhs ) { x-=rhs.x; y-=rhs.y; return *this;}

        //______________________________________________________________________
        //
        // multiplication
        //______________________________________________________________________
        inline friend point2d operator*(param_type a,const point2d &p) { return point2d(a*p.x,a*p.y); }
        inline point2d & operator*=(param_type a) { x*=a; y*=a; return *this; }

        //______________________________________________________________________
        //
        // division
        //______________________________________________________________________
        inline friend point2d operator/(const point2d &p,param_type a) { return point2d(p.x/a,p.y/a); }
        inline point2d & operator/=(param_type a) { x/=a; y/=a; return *this; }


        //______________________________________________________________________
        //
        // vectorial
        //______________________________________________________________________
        inline friend const_type operator*(const point2d &lhs, const point2d &rhs)
        {
            return (lhs.x*rhs.x) + (lhs.y*rhs.y);
        }

        //______________________________________________________________________
        //
        // other
        //______________________________________________________________________
        inline type norm2() const throw() { return x*x + y*y; }
        inline size_t      size() const throw() { return 2; }
        inline type       &operator[](const size_t indx) throw()       { assert(1==indx||2==indx); return *((&x-1)+indx); }
        inline const_type &operator[](const size_t indx) const throw() { assert(1==indx||2==indx); return *((&x-1)+indx); }
        inline type norm() const throw() { return math::Sqrt(x*x+y*y); }

        //! equal operator for integer types only
        inline friend bool operator==(const point2d &lhs, const point2d &rhs) throw()
        {
            return (lhs.x==rhs.x) && (lhs.y==rhs.y);
        }

        //! for integer types only...
        inline friend bool operator!=(const point2d &lhs, const point2d &rhs) throw()
        {
            return (lhs.x!=rhs.x) || (lhs.y!=rhs.y);
        }

        //! on unit circle, T=float|double
        static inline
        point2d on_unit_circle( urand32 &ran ) throw()
        {
            const T angle = math::numeric<T>::two_pi * ran.get<T>();
            return point2d( math::Cos(angle), math::Sin(angle) );
        }

        static inline
        point2d in_unit_disk( urand32 &ran ) throw()
        {
            while(true)
            {
                const T X = ran.sym1<T>();
                const T Y = ran.sym1<T>();
                const T r2 = (X*X) + (Y*Y);
                if(r2>=T(1)) continue;
                return point2d(X,Y);
            }
        }

        inline type __prod() const throw() { return x*y; }
        inline type __sum()  const throw() { return x+y; }
        
    };


}

YOCTO_SUPPORT_C_STYLE_OPS(point2d<float>);
YOCTO_SUPPORT_C_STYLE_OPS(point2d<double>);
YOCTO_SUPPORT_C_STYLE_OPS(point2d<unit_t>);

#endif

