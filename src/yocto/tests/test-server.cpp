#include "yocto/threading/scheme/server.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/wtime.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;

namespace  {

    static double sleeping_time = 0.2;

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
                wtime::sleep(sleeping_time);

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

    size_t n = 8;

    if(argc>1)
    {
        n = strconv::to_size(argv[1],"n");
    }

    if(argc>2)
    {
        sleeping_time = strconv::to_double(argv[2],"sleeping_time");
    }

    wtime chrono;
    std::cerr << "Sequential Code" << std::endl;
    chrono.start();
    for(size_t i=0;i<n;++i)
    {
        seqsrv.enqueue(k);
    }
    double seq_time = chrono.query();

    std::cerr << "seq_time=" << seq_time * 1000.0 << std::endl;

    std::cerr << "Parallel Code" << std::endl;
    chrono.start();
    for(size_t i=0;i<n;++i)
    {
        parsrv.enqueue(k);
    }
    parsrv.flush();

    double par_time = chrono.query();
    std::cerr << "par_time=" << par_time * 1000.0 << std::endl;

    std::cerr << std::endl;
    std::cerr << "efficiency: " << ipso::efficiency(seq_time/par_time,parsrv.size) << "%" << std::endl;

    std::cerr << std::endl;
    std::cerr << "using batch..." << std::endl;

    vector<threading::kernel> batch(n,as_capacity);
    for(size_t i=1;i<=n;++i)  batch.push_back(k);

    std::cerr << "Sequential Code" << std::endl;
    chrono.start();
    seqsrv.enqueue_all(batch);
    seq_time = chrono.query();

    std::cerr << "seq_time=" << seq_time * 1000.0 << std::endl;
    std::cerr << "Parallel Code" << std::endl;
    chrono.start();
    parsrv.enqueue_all(batch);
    parsrv.flush();

    par_time = chrono.query();
    std::cerr << "par_time=" << par_time * 1000.0 << std::endl;

    std::cerr << std::endl;
    std::cerr << "efficiency: " << ipso::efficiency(seq_time/par_time,parsrv.size) << "%" << std::endl;
    
}
YOCTO_UNIT_TEST_DONE()
