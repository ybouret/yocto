#include "yocto/threading/scheme/simd.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand32.hpp"
#include "yocto/sys/wtime.hpp"

using namespace yocto;

namespace {

    class Todo
    {
    public:
        double      s;

        inline Todo() throw() : s(0)
        {
        }

        inline ~Todo() throw()
        {
        }

        inline void proc( threading::context &ctx ) throw()
        {
            rand32_kiss r;
            r.seed(ctx.rank);
            double sum    = 0;
            size_t offset = 0;
            size_t length = count;
            ctx.split(offset,length);
            {
                YOCTO_LOCK(ctx.access);
                std::cerr << ctx.size << "." << ctx.rank << ": " << offset << "+" << length << std::endl;
            }
            for(size_t i=0;i<length;++i)
            {
                sum += r.get<double>();
            }
            // warning: memory allocation is not thread safe ? ctx.make<double>(sum);
        }

        static size_t count;

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Todo);
    };


    size_t Todo::count = 100000000;
}


YOCTO_UNIT_TEST_IMPL(SIMD)
{
    wtime chrono;

    Todo todo;

    threading::SIMD1 seq;
    std::cerr << "Sequential..." << std::endl;
    chrono.start();
    seq.load( &todo, & Todo::proc );
    const double seq_ell = chrono.query();
    std::cerr << "done...in " << (seq_ell)*1000.0 << " ms" << std::endl << std::endl;

    std::cerr << "Parallel..." << std::endl;
    threading::SIMD par(true);
    chrono.start();
    par.load( &todo, & Todo::proc );
    const double par_ell = chrono.query();
    std::cerr << "done...in " << (par_ell)*1000.0 << " ms" << std::endl << std::endl;

    const double speed_up = seq_ell/par_ell;
    const double eff      = parallel::efficiency(speed_up,par.size);
    std::cerr << "efficiency=" << eff << "%" << std::endl << std::endl;

}
YOCTO_UNIT_TEST_DONE()




