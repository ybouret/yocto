#include "yocto/math/fcn/newton.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/code/rand.hpp"

using namespace yocto;
using namespace math;

namespace
{

    class MySystem
    {
    public:
        MySystem()
        {
        }

        ~MySystem()
        {
        }

        inline  void getF(array<double>       &F,
                          const array<double> &X)
        {
            F[1] = 1.0 - tao::norm_sq(X);
            F[2] = X[2] - X[1];
        }

        inline void getJ(matrix<double>      &J,
                         const array<double> &X)
        {
            //getF(F,X);
            J[1][1] = -2 * X[1]; J[1][2] = -2 * X[2];
            J[2][1] = -1;        J[2][2] = 1;
        }


    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(MySystem);
    };

}

YOCTO_UNIT_TEST_IMPL(newton)
{
    MySystem ms;
    Newton<double>::Function F( &ms, & MySystem::getF );
    Newton<double>::Jacobian J( &ms, & MySystem::getJ );

    Newton<double> newton(2);
    vector<double> X(2);

    for(size_t i=X.size();i>0;--i)
    {
        X[i] = 1.0 - 2.0 * alea<double>();
    }

    newton.run(F, J, X);
    
}
YOCTO_UNIT_TEST_DONE()
