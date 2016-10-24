#include "yocto/threading/scheme/simd.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand32.hpp"

using namespace yocto;

namespace {

    class Todo
    {
    public:
        rand32_kiss r;
        double      s;

        inline Todo() throw() : r(), s(0)
        {
        }

        inline ~Todo() throw()
        {
        }

        inline void proc( threading::context &ctx ) throw()
        {
            r.seed(ctx.rank);
            double sum = 0;
            size_t offset = 0;
            size_t length = count;
            ctx.split(offset,length);
            for(size_t i=0;i<length;++i)
            {
                sum += r.get<double>();
            }
            ctx.make<double>(sum);
        }

        static size_t count;

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Todo);
    };


    size_t Todo::count = 100000000;
}



YOCTO_UNIT_TEST_IMPL(schemes)
{
    Todo todo;

    threading::SIMD1 seq;
    std::cerr << "Sequential..." << std::endl;
    seq.load( &todo, & Todo::proc );
    std::cerr << "done..." << std::endl << std::endl;

    std::cerr << "Parallel..." << std::endl;
    threading::SIMD par(true);
    par.load( &todo, & Todo::proc );

}
YOCTO_UNIT_TEST_DONE()
