#include "yocto/container/tuple.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_PAIR_DECL(STANDARD,Coord,int,x,double,y);

YOCTO_DISABLE_ASSIGN(Coord);
YOCTO_PAIR_END();

YOCTO_TRIPLE_DECL(STANDARD,Coord3,int,x,const double,y,long,z);

YOCTO_DISABLE_ASSIGN(Coord3);
YOCTO_TRIPLE_END();

YOCTO_QUAD_DECL(STANDARD,Qat,int,x,const double,y,long,z,float,w);

inline Qat() throw() : x(0), y(0), z(0), w(0) {}

YOCTO_DISABLE_ASSIGN(Qat);
YOCTO_QUAD_END();


YOCTO_PENTUPLE_DECL(STANDARD,Pentuple,int,a,int, b, int, c, int,d,int,e);
YOCTO_PENTUPLE_END();

YOCTO_SEXTUPLE_DECL(STANDARD,Sextuple,int,a,int, b, int, c, int,d,int,e,int,f);
YOCTO_SEXTUPLE_END();


YOCTO_UNIT_TEST_IMPL(tuple)
{
    Coord c1(1,2);
    Coord c2(c1);
    
    
    std::cerr << "c1=" << c1 << std::endl;
    std::cerr << "c2=" << c2 << std::endl;
    
    Coord3 X1(1,2,3);
    Coord3 X2(X1);
    
    std::cerr << "X1=" << X1 << std::endl;
    std::cerr << "X2=" << X2 << std::endl;

    Qat Q1;
    Q1.x = 2;
    Q1.w = 1.0;
    Qat Q2(Q1);
    std::cerr << "Q1=" << Q1 << std::endl;
    std::cerr << "Q1=" << Q2 << std::endl;

}
YOCTO_UNIT_TEST_DONE()
