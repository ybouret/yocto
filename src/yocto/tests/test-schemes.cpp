#include "yocto/threading/scheme/simd.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

namespace {

    class Todo
    {
    public:
        inline Todo() throw()
        {
        }

        inline ~Todo() throw()
        {
        }

        inline void proc( threading::context &ctx ) throw()
        {
            {
                YOCTO_LOCK(ctx.access);
                std::cerr << "@proc " << ctx.size << "." << ctx.rank << std::endl;
            }


        }

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Todo);
    };

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
