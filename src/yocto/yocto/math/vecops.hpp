#ifndef YOCTO_MATH_VECOPS_INCLUDED
#define YOCTO_MATH_VECOPS_INCLUDED 1

#include "yocto/math/point3d.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    template <size_t DIM>
    struct vecops;


    template <>
    struct vecops<1>
    {
        template <typename T>
        static inline bool eq(const T *lhs, const T *rhs) throw()
        {
            return (lhs[0] == rhs[0]);
        }

        template <typename T>
        static inline T sum(const T *lhs) throw()
        {
            return lhs[0];
        }

        template <typename T>
        static inline T prod(const T *lhs) throw()
        {
            return lhs[0];
        }

        template <typename T>
        static inline T max(const T *lhs) throw()
        {
            return lhs[0];
        }

        template <typename T>
        static inline T min(const T *lhs) throw()
        {
            return lhs[0];
        }

        template <typename T>
        static inline void dec(T *lhs) throw()
        {
            --lhs[0];
        }

        template <typename T>
        static inline void mul(T *v, const T fac) throw()
        {
            v[0] *= fac;
        }

        template <typename T,typename U>
        static inline void muladd(T *v, const T fac, const U *u) throw()
        {
            v[0] += fac * T(u[0]);
        }

    };

    template <>
    struct vecops<2>
    {
        template <typename T>
        static inline bool eq(const T *lhs, const T *rhs) throw()
        {
            return (lhs[0] == rhs[0]) && (lhs[1]==rhs[1]);
        }

        template <typename T>
        static inline T sum(const T *lhs) throw()
        {
            return lhs[0]+lhs[1];
        }

        template <typename T>
        static inline T prod(const T *lhs) throw()
        {
            return lhs[0]*lhs[1];
        }

        template <typename T>
        static inline T max(const T *lhs) throw()
        {
            return max_of<T>(lhs[0],lhs[1]);
        }

        template <typename T>
        static inline T min(const T *lhs) throw()
        {
            return min_of<T>(lhs[0],lhs[1]);
        }

        template <typename T>
        static inline void dec(T *lhs) throw()
        {
            --lhs[0];
            --lhs[1];
        }

        template <typename T>
        static inline void mul(T *v, const T fac) throw()
        {
            v[0] *= fac;
            v[1] *= fac;
        }

        template <typename T,typename U>
        static inline void muladd(T *v, const T fac, const U *u) throw()
        {
            v[0] += fac * T(u[0]);
            v[1] += fac * T(u[1]);
        }
    };


    template <>
    struct vecops<3>
    {
        template <typename T>
        static inline bool eq(const T *lhs, const T *rhs) throw()
        {
            return (lhs[0]==rhs[0]) && (lhs[1]==rhs[1]) && (lhs[2]==rhs[2]);
        }

        template <typename T>
        static inline T sum(const T *lhs) throw()
        {
            return lhs[0]+lhs[1]+lhs[2];
        }

        template <typename T>
        static inline T prod(const T *lhs) throw()
        {
            return lhs[0]*lhs[1]*lhs[2];
        }

        template <typename T>
        static inline T max(const T *lhs) throw()
        {
            return max_of<T>( max_of<T>(lhs[0],lhs[1]), lhs[2]);
        }

        template <typename T>
        static inline T min(const T *lhs) throw()
        {
            return min_of<T>( min_of<T>(lhs[0],lhs[1]), lhs[2]);
        }

        template <typename T>
        static inline void dec(T *lhs) throw()
        {
            --lhs[0];
            --lhs[1];
            --lhs[2];
        }

        template <typename T>
        static inline void mul(T *v, const T fac) throw()
        {
            v[0] *= fac;
            v[1] *= fac;
            v[2] *= fac;
        }

        template <typename T,typename U>
        static inline void muladd(T *v, const T fac, const U *u) throw()
        {
            v[0] += fac * T(u[0]);
            v[1] += fac * T(u[1]);
            v[2] += fac * T(u[2]);
        }
    };

#define YOCTO_VECOPS_DIM(SCALAR,VECTOR) (sizeof(VECTOR)/sizeof(SCALAR))

#define YOCTO_VECOPS_OP(OP,SCALAR,VECTOR) vecops<YOCTO_VECOPS_DIM(SCALAR,VECTOR)>::OP

#define YOCTO_VECOPS(PREFIX,SCALAR,VECTOR) \
template <typename VECTOR> inline SCALAR &PREFIX(VECTOR &V,const size_t dim) throw() \
{ assert(dim<YOCTO_VECOPS_DIM(SCALAR,VECTOR)); return *(((SCALAR *)&V)+dim); }       \
\
template <typename VECTOR> inline const SCALAR &PREFIX(const VECTOR &V,const size_t dim) throw() \
{ assert(dim<YOCTO_VECOPS_DIM(SCALAR,VECTOR)); return *(((const SCALAR *)&V)+dim); }\
\
template <typename VECTOR> inline bool  PREFIX##_eq(const VECTOR &lhs, const VECTOR &rhs) throw()\
{ return YOCTO_VECOPS_OP(eq,SCALAR,VECTOR)((const SCALAR *)&lhs,(const SCALAR *)&rhs); }\
\
template <typename VECTOR> inline SCALAR PREFIX##_sum(const VECTOR &lhs) throw()\
{ return YOCTO_VECOPS_OP(sum,SCALAR,VECTOR)((const SCALAR *)&lhs); }\
\
template <typename VECTOR> inline SCALAR PREFIX##_prod(const VECTOR &lhs) throw()\
{ return YOCTO_VECOPS_OP(prod,SCALAR,VECTOR)((const SCALAR *)&lhs); }\
\
template <typename VECTOR> inline SCALAR PREFIX##_max(const VECTOR &lhs) throw()\
{ return YOCTO_VECOPS_OP(max,SCALAR,VECTOR)((const SCALAR *)&lhs); }\
\
template <typename VECTOR> inline SCALAR PREFIX##_min(const VECTOR &lhs) throw()\
{ return YOCTO_VECOPS_OP(min,SCALAR,VECTOR)((const SCALAR *)&lhs); }\
\
template <typename VECTOR> inline void PREFIX##_dec(VECTOR &lhs) throw()\
{  YOCTO_VECOPS_OP(dec,SCALAR,VECTOR)( ( SCALAR *)&lhs ); }


}


#endif

