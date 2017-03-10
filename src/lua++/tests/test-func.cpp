#include "yocto/lua/lua-maths.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(func)
{
    Lua::State::Pointer L( new Lua::State() );
    L->DumpStack();

    L->DoString("function f(x) return x+1; end");
    for(int i=1;i<argc;++i)
    {
        L->DoString(argv[i]);
    }

    Lua::Function<double> f(L,"f");
    for(double x=0;x<=1;x+=0.1)
    {
        std::cerr << "f(" << x << ")=" << f(x) << std::endl;
    }

}
YOCTO_UNIT_TEST_DONE()
