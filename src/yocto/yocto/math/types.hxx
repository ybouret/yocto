#ifndef YOCTO_MATH_TYPES__INCLUDED
#define YOCTO_MATH_TYPES__INCLUDED 1

#define YOCTO_MATH_EXTERN_DECL 1

#if defined(__GNUC__) && (__GNUC__<=3)
#undef  YOCTO_MATH_EXTERN_DECL
#define YOCTO_MATH_EXTERN_DECL 0
#endif

#if (1==YOCTO_MATH_EXTERN_DECL)

#define _YOCTO_MATH_EXTERN(TYPE,NAME) \
extern template const TYPE yocto::math::numeric<TYPE>::NAME

#define YOCTO_MATH_EXTERN(NAME) \
_YOCTO_MATH_EXTERN(float,NAME); \
_YOCTO_MATH_EXTERN(double,NAME)

#define _YOCTO_MATH_EXTERN_Z(TYPE,NAME) extern template const yocto::math::complex<TYPE> yocto::math::numeric< yocto::math::complex<TYPE> >::NAME

#define YOCTO_MATH_EXTERN_Z(NAME) \
_YOCTO_MATH_EXTERN_Z(float,NAME); \
_YOCTO_MATH_EXTERN_Z(double,NAME)

YOCTO_MATH_EXTERN(zero);
YOCTO_MATH_EXTERN_Z(zero);



YOCTO_MATH_EXTERN(epsilon);
YOCTO_MATH_EXTERN(sqrt_epsilon);
YOCTO_MATH_EXTERN(minimum);
YOCTO_MATH_EXTERN(maximum);
YOCTO_MATH_EXTERN(pi);
YOCTO_MATH_EXTERN(two_pi);
YOCTO_MATH_EXTERN(half_pi);
YOCTO_MATH_EXTERN(ftol);
YOCTO_MATH_EXTERN(sqrt_ftol);
YOCTO_MATH_EXTERN(min_exp);
YOCTO_MATH_EXTERN(max_exp);
YOCTO_MATH_EXTERN(min_10_exp);
YOCTO_MATH_EXTERN(max_10_exp);
YOCTO_MATH_EXTERN(tiny);
YOCTO_MATH_EXTERN(huge);
YOCTO_MATH_EXTERN(sqrt_tiny);
YOCTO_MATH_EXTERN(gold);
YOCTO_MATH_EXTERN(gold_inv);

#endif

#endif
