#ifndef YOCTO_TUPLE_INCLUDED
#define YOCTO_TUPLE_INCLUDED 1

#include "yocto/type/traits.hpp"
#include "yocto/object.hpp"
#include <iostream>

#define YOCTO_TUPLE_INI()   {
#define YOCTO_TUPLE_END()   }

#define YOCTO_TUPLE_STANDARD(T) type_traits< T >::parameter_type
#define YOCTO_TUPLE_TEMPLATE(T) typename type_traits< T >::parameter_type

#define YOCTO_PAIR_DECL(PARAM,CLASS,T1,N1,T2,N2) \
class CLASS : public virtual object \
YOCTO_TUPLE_INI() \
public:\
typedef YOCTO_TUPLE_##PARAM(T1) PARAM1;\
typedef YOCTO_TUPLE_##PARAM(T2) PARAM2;\
inline CLASS( PARAM1 P1, PARAM2 P2) :\
N1(P1),N2(P2) {} \
inline CLASS(const CLASS &other) : object(),\
N1(other.N1), N2(other.N2) {}\
inline virtual ~CLASS() throw() {} \
friend inline \
std::ostream & operator<<( std::ostream &pair_os, const CLASS &self )\
{\
pair_os << '(' << #N1 << '=' << self.N1 << ',' << #N2 << '=' << self.N2 << ')';\
return pair_os;\
}\
friend inline bool operator==(const CLASS &lhs, const CLASS &rhs) throw()\
{\
return (lhs.N1==rhs.N1) && (lhs.N2==rhs.N2);\
}\
friend inline bool operator!=(const CLASS &lhs, const CLASS &rhs) throw()\
{\
return (lhs.N1!=rhs.N1) || (lhs.N2!=rhs.N2);\
}\
T1 N1;\
T2 N2

#define YOCTO_PAIR_END() YOCTO_TUPLE_END()


#define YOCTO_TRIPLE_DECL(PARAM,CLASS,T1,N1,T2,N2,T3,N3) \
class CLASS : public virtual object \
YOCTO_TUPLE_INI() \
public:\
typedef YOCTO_TUPLE_##PARAM(T1) PARAM1;\
typedef YOCTO_TUPLE_##PARAM(T2) PARAM2;\
typedef YOCTO_TUPLE_##PARAM(T3) PARAM3;\
inline CLASS( PARAM1 P1, PARAM2 P2, PARAM3 P3) :\
N1(P1),N2(P2),N3(P3) {} \
inline CLASS(const CLASS &other) : object(),\
N1(other.N1), N2(other.N2), N3(other.N3) {}\
inline virtual ~CLASS() throw() {} \
friend inline \
std::ostream & operator<<( std::ostream &os, const CLASS &self )\
{\
os << '(' << #N1 << '=' << self.N1 << ',' << #N2 << '=' << self.N2 << ',' << #N3 << '=' << self.N3 << ')';\
return os;\
}\
T1 N1;\
T2 N2;\
T3 N3

#define YOCTO_TRIPLE_END() YOCTO_TUPLE_END()

#define YOCTO_QUAD_DECL(PARAM,CLASS,T1,N1,T2,N2,T3,N3,T4,N4) \
class CLASS : public virtual object \
YOCTO_TUPLE_INI() \
public:\
typedef YOCTO_TUPLE_##PARAM(T1) PARAM1;\
typedef YOCTO_TUPLE_##PARAM(T2) PARAM2;\
typedef YOCTO_TUPLE_##PARAM(T3) PARAM3;\
typedef YOCTO_TUPLE_##PARAM(T4) PARAM4;\
inline CLASS( PARAM1 P1, PARAM2 P2, PARAM3 P3, PARAM4 P4) :\
N1(P1),N2(P2),N3(P3),N4(P4) {} \
inline CLASS(const CLASS &other) : object(),\
N1(other.N1), N2(other.N2), N3(other.N3),N4(other.N4) {}\
inline virtual ~CLASS() throw() {} \
friend inline \
std::ostream & operator<<( std::ostream &os, const CLASS &self )\
{\
os << '(' << #N1 << '=' << self.N1 << ',' << #N2 << '=' << self.N2 << ',' << #N3 << '=' << self.N3 << ',' << #N4 << '=' << self.N4 << ')';\
return os;\
}\
T1 N1;\
T2 N2;\
T3 N3;\
T4 N4

#define YOCTO_QUAD_END() YOCTO_TUPLE_END()


#define YOCTO_PENTUPLE_DECL(PARAM,CLASS,T1,N1,T2,N2,T3,N3,T4,N4,T5,N5) \
class CLASS : public virtual object \
YOCTO_TUPLE_INI() \
public:\
typedef YOCTO_TUPLE_##PARAM(T1) PARAM1;\
typedef YOCTO_TUPLE_##PARAM(T2) PARAM2;\
typedef YOCTO_TUPLE_##PARAM(T3) PARAM3;\
typedef YOCTO_TUPLE_##PARAM(T4) PARAM4;\
typedef YOCTO_TUPLE_##PARAM(T5) PARAM5;\
inline CLASS( PARAM1 P1, PARAM2 P2, PARAM3 P3, PARAM4 P4, PARAM5 P5) :\
N1(P1),N2(P2),N3(P3),N4(P4),N5(P5) {} \
inline CLASS(const CLASS &other) : object(),\
N1(other.N1),N2(other.N2),N3(other.N3),N4(other.N4),N5(other.N5) {}\
inline virtual ~CLASS() throw() {} \
friend inline \
std::ostream & operator<<( std::ostream &os, const CLASS &self )\
{\
os << '(' << self.N1 << ',' << self.N2 << ',' << self.N3 << ',' << self.N4 << ',' << self.N5 << ')';\
return os;\
}\
T1 N1;\
T2 N2;\
T3 N3;\
T4 N4;\
T5 N5

#define YOCTO_PENTUPLE_END() YOCTO_TUPLE_END()

#define YOCTO_SEXTUPLE_DECL(PARAM,CLASS,T1,N1,T2,N2,T3,N3,T4,N4,T5,N5,T6,N6) \
class CLASS : public virtual object \
YOCTO_TUPLE_INI() \
public:\
typedef YOCTO_TUPLE_##PARAM(T1) PARAM1;\
typedef YOCTO_TUPLE_##PARAM(T2) PARAM2;\
typedef YOCTO_TUPLE_##PARAM(T3) PARAM3;\
typedef YOCTO_TUPLE_##PARAM(T4) PARAM4;\
typedef YOCTO_TUPLE_##PARAM(T5) PARAM5;\
typedef YOCTO_TUPLE_##PARAM(T6) PARAM6;\
inline CLASS( PARAM1 P1, PARAM2 P2, PARAM3 P3, PARAM4 P4, PARAM5 P5, PARAM6 P6) :\
N1(P1),N2(P2),N3(P3),N4(P4),N5(P5),N6(P6) {} \
inline CLASS(const CLASS &other) : object(),\
N1(other.N1),N2(other.N2),N3(other.N3),N4(other.N4),N5(other.N5),N6(other.N6) {}\
inline virtual ~CLASS() throw() {} \
friend inline \
std::ostream & operator<<( std::ostream &os, const CLASS &self )\
{\
os << '(' << self.N1 << ',' << self.N2 << ',' << self.N3 << ',' << self.N4 << ',' << self.N5 << ',' << self.N6 << ')';\
return os;\
}\
T1 N1;\
T2 N2;\
T3 N3;\
T4 N4;\
T5 N5;\
T6 N6

#define YOCTO_SEXTUPLE_END() YOCTO_TUPLE_END()

#define YOCTO_HEPTUPLE_DECL(PARAM,CLASS,T1,N1,T2,N2,T3,N3,T4,N4,T5,N5,T6,N6,T7,N7) \
class CLASS : public virtual object \
YOCTO_TUPLE_INI() \
public:\
typedef YOCTO_TUPLE_##PARAM(T1) PARAM1;\
typedef YOCTO_TUPLE_##PARAM(T2) PARAM2;\
typedef YOCTO_TUPLE_##PARAM(T3) PARAM3;\
typedef YOCTO_TUPLE_##PARAM(T4) PARAM4;\
typedef YOCTO_TUPLE_##PARAM(T5) PARAM5;\
typedef YOCTO_TUPLE_##PARAM(T6) PARAM6;\
typedef YOCTO_TUPLE_##PARAM(T7) PARAM7;\
inline CLASS( PARAM1 P1, PARAM2 P2, PARAM3 P3, PARAM4 P4, PARAM5 P5, PARAM6 P6, PARAM7 P7) :\
N1(P1),N2(P2),N3(P3),N4(P4),N5(P5),N6(P6),N7(P7) {} \
inline CLASS(const CLASS &other) : object(),\
N1(other.N1),N2(other.N2),N3(other.N3),N4(other.N4),N5(other.N5),N6(other.N6),N7(other.N7) {}\
inline virtual ~CLASS() throw() {} \
friend inline \
std::ostream & operator<<( std::ostream &os, const CLASS &self )\
{\
os << '(' << self.N1 << ',' << self.N2 << ',' << self.N3 << ',' << self.N4 << ',' << self.N5 << ',' << self.N6 << ',' << self.N7 <<')';\
return os;\
}\
T1 N1;\
T2 N2;\
T3 N3;\
T4 N4;\
T5 N5;\
T6 N6;\
T7 N7

#define YOCTO_HEPTUPLE_END() YOCTO_TUPLE_END()



#endif

