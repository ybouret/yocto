#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/math/fcn/derivatives.hpp"
#include "yocto/sequence/vector.hpp"

#include <typeinfo>

using namespace yocto;
using namespace math;

namespace {

    template <typename T>
    static inline T F(T x )
    {
        return Sin(0.3*x);
    }




    
    static unsigned Counts = 0;
    template <typename T>
    static inline T Cos2( T x )
    {
        ++Counts;
        return Cos( x*x );
    }

    class Dummy
    {
    public:
        explicit Dummy() {}
        virtual ~Dummy() throw() {}

        template <typename T>
        T Function( T x )
        {
            return Cos2<T>(x);
        }

        template <typename T>
        T Field( const array<T> &x )
        {
            T ans = 0;
            for(size_t i=1;i<=x.size();++i)
            {
                ans += Square(x[i]);
            }
            return ans/2;
        }

        template <typename T>
        T FitFn( const T x, const array<T> &a )
        {
            return Function<T>(x) * Field<T>(a) ;
        }


        template <typename T>
        static inline void Test()
        {
            std::cerr << "Testing <" << typeid(T).name() << ">" << std::endl;
            derivatives<T> drvs;
            Dummy dum;

            {
                typename numeric<T>::function F( &dum, & Dummy::Function<T> );
                for(size_t i=10000;i>0;--i)
                {
                    (void)drvs.compute(F,10*alea.get<T>(),1e-4);
                }
            }

            {
                typename numeric<T>::scalar_field        F( &dum, & Dummy::Field<T> );
                typename numeric<T>::parametric_function P( &dum, & Dummy::FitFn<T> );
                vector<T> x(4,0);
                vector<T> h(x.size(),1e-4);
                vector<T> g(x.size());

                for(size_t i=10;i>0;--i)
                {
                    for(size_t j=x.size();j>0;--j)
                    {
                        x[j] = T(100)*(alea.get<T>()-T(0.5));
                    }
                    drvs.gradient(g,F,x,h);
                    std::cerr << x << " => " << g << std::endl;
                    std::cerr << "|_: " << drvs.compute(P, 10*alea.get<T>(), x, 1e-4) << std::endl;
                    drvs.gradient(g,P,10*alea.get<T>(), x, h);
                    std::cerr << "|_: " << g << std::endl;
                }

            }


        }

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Dummy);
    };

}

#include "yocto/math/types.hxx"

template <typename T>
static inline void show_der( const derivatives<T> &diff )
{
    std::cerr << "derivatives<" << typeid(T).name() << ">" << std::endl;
    std::cerr << "size    : " << sizeof(derivatives<T>) << " bytes" << std::endl;
    std::cerr << "epsilon : " << numeric<T>::epsilon << std::endl;
    std::cerr << "max_ftol: " << diff.max_ftol << std::endl;
    std::cerr << "opt_step: " << diff.opt_step << std::endl;
    std::cerr << "sizeof(matrix)=" << sizeof(matrix<T>) << std::endl;
    std::cerr << std::endl;
}


YOCTO_UNIT_TEST_IMPL(drvs)
{

    {
        derivatives<float>  diff_f;
        derivatives<double> diff_d;
        show_der(diff_f);
        show_der(diff_d);

        numeric<double>::function F  = cfunctor( Cos2<double> );
        const double xmax = 10;
        const double step = 0.02;
        {
            double err = 0;
            ios::ocstream fp("diff0.dat",false);
            for( double x=0; x <= xmax; x += step)
            {
                Counts = 0;
                const double dFdx = diff_d.diff(F, x, 0.001, err);
                fp("%g %g %g %u %g\n", x, F(x), dFdx, Counts, err);
            }
        }

        {
            ios::ocstream fp("diff1.dat",false);
            for( double x=0; x <= xmax; x += step)
            {
                Counts = 0;
                const double dFdx = diff_d.compute(F, x, 0.001);
                fp("%g %g %g %u\n", x, F(x), dFdx, Counts);
            }
        }
    }

    Dummy::Test<float>();
    Dummy::Test<double>();



    
}
YOCTO_UNIT_TEST_DONE()


