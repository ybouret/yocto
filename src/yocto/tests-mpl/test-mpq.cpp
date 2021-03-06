#include "yocto/mpl/rational.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/math/core/determinant.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/code/alea.hpp"
#include "yocto/math/core/tao.hpp"

using namespace yocto;
using namespace mpl;

YOCTO_UNIT_TEST_IMPL(mpq)
{
    mpq r = 2;
    std::cerr << "r=" << r << std::endl;
    mpq x(1,2);
    mpq y(1,3);
    std::cerr << x << "-" << y << "=" << x-y << std::endl;
    matrix<mpq>    M(5);
    matrix<double> A(5);

    for(mpq i=mpq(11,10);i<10;++i)
    {
        const mpq i2 = mpq::sqr(i);
        std::cerr << "i=" << i << ", i2=" << i2 << std::endl;
    }

    for(mpq i=mpq(122,10);i>=0;--i)
    {
        const mpq i2 = mpq::sqr(i);
        std::cerr << "i=" << i << ", i2=" << i2 << std::endl;
        
    }

    for(size_t i=1;i<=M.rows;++i)
    {
        for(size_t j=1;j<=M.cols;++j)
        {
            const int tmp = alea.full<int16_t>();
            M[i][j] = tmp;
            A[i][j] = tmp;
        }
    }
    std::cerr << "M=" << M << std::endl;
    const mpq dd = math::determinant(M);
    std::cerr << "dd=" << dd << "; " << math::determinant(A) << std::endl;
    
    matrix<mpq> M0(M);
    matrix<mpq> IM(M.rows);
    IM.ld1();
    if( ! math::LU<mpq>::build(M) )
    {
        std::cerr << "Cannot LU..." << std::endl;
    }
    else
    {
        math::LU<mpq>::solve(M,IM);
        std::cerr << "IM=" << IM << std::endl;
        matrix<double> IMr(M.cols,M.rows);
        for(size_t i=1;i<=M.rows;++i)
        {
            for(size_t j=1;j<=M.cols;++j)
            {
                IMr[i][j] = IM[i][j].to_double();
            }
        }
        std::cerr << "IMr=" << IMr << std::endl;
        matrix<mpq> P(M.rows);
        math::tao::mmul(P, M0, IM);
        std::cerr << "P=" << P << std::endl;
        matrix<double> Pr(M.rows);
        math::tao::mmul(Pr,A,IMr);
        std::cerr << "Pr=" << Pr << std::endl;
    }


}
YOCTO_UNIT_TEST_DONE()
