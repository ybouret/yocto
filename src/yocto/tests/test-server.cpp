#include "yocto/threading/scheme/server.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/wtime.hpp"

using namespace yocto;

namespace  {

    class DoSomething
    {
    public:
        explicit DoSomething() throw() {}
        virtual ~DoSomething() throw() {}


        void Run( threading::context &ctx ) throw()
        {
            {
                YOCTO_LOCK(ctx.access);
                std::cerr << "Run on " << ctx.size << "." << ctx.rank << std::endl;
            }
                wtime::sleep(0.5);

        }

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(DoSomething);
    };

}

YOCTO_UNIT_TEST_IMPL(server)
{
    threading::seq_server seqsrv;
    threading::par_server parsrv(false);

    DoSomething       dummy;
    threading::kernel k( & dummy, & DoSomething::Run );

    const size_t n = 8;
    wtime chrono;

    chrono.start();
    for(size_t i=0;i<n;++i)
    {
        seqsrv.enqueue(k);
    }
    const double seq_time = chrono.query();

    std::cerr << "seq_time=" << seq_time * 1000.0 << std::endl;

    chrono.start();
    for(size_t i=0;i<n;++i)
    {
        parsrv.enqueue(k);
    }
    parsrv.flush();

    const double par_time = chrono.query();
    std::cerr << "par_time=" << par_time * 1000.0 << std::endl;

    std::cerr << std::endl;
    std::cerr << "efficiency: " << parallel::efficiency(seq_time/par_time,parsrv.size) << "%" << std::endl;

}
YOCTO_UNIT_TEST_DONE()
