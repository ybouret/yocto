#include "yocto/utest/run.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/opt/bracket.hpp"

#include <typeinfo>

using namespace yocto;
using namespace math;

static size_t call_counts = 0;

template <typename T>
static inline T F( T x )
{
    ++call_counts;
    //return 7.1-Cos(x+T(0.23));
    //return 1-Cos(x+0.1);
    return T(3)-Cos(x+T(0.2)*x*x+T(0.1));
}

template <typename T>
static inline void optim(T xtol)
{
    call_counts = 0;
    typename numeric<T>::function func( cfunctor(F<float>) );
    triplet<T> x = { 1, 0 , -1};
    triplet<T> f = { func(x.a), 0, func(x.c) };
    bracket<T>::inside( func, x, f );
    std::cerr << "x=" << x << std::endl;
    std::cerr << "f=" << f << std::endl;
    
	optimize1D<T>::run(func,x,f,xtol);
    std::cerr << "min@" << x.b << " = " << f.b << std::endl;
    std::cerr << "\t\t#calls=" << call_counts << std::endl;
}

#include "yocto/string/conv.hpp"


YOCTO_UNIT_TEST_IMPL(opt)
{
    
    double xtol = 1e-4;
    if(argc>1)
    {
        xtol = strconv::to_double(argv[1],"xtol");
    }
 
    std::cerr << "Double: " << std::endl;
    optim<double>(xtol);
}
YOCTO_UNIT_TEST_DONE()

