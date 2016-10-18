#include "yocto/threading/engine.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/wtime.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/code/rand32.hpp"

using namespace yocto;
using namespace threading;

namespace {


    static size_t nIter = 131072;

    class Work
    {
    public:
        size_t    value;
        double    sum;
        static   double secs;

        inline  Work( int v ) throw() : value(v),sum(0) {}
        inline ~Work() throw() {}
        inline  Work( const Work &w ) throw() : value( w.value ), sum(w.sum) {}

        inline void operator()(context &ctx)
        {
            {
                YOCTO_LOCK(ctx.access);
                std::cerr << "Working @" << value << "..." << std::endl;
                std::cerr.flush();
            }
            rand32_kiss r;
            {
                YOCTO_LOCK(ctx.access);
                r.wseed();
            }
            wtime chrono;
            chrono.start();
            while(chrono.query()<secs)
            {
                for(size_t i=0;i<nIter;++i)
                {
                    sum += r.get<double>();
                    (void)r.next();
                }
            }
        }

    private:
        YOCTO_DISABLE_ASSIGN(Work);
    };

    double Work::secs = 0.5;
}

YOCTO_UNIT_TEST_IMPL(engine)
{
    size_t nj = 1;
    if(argc>1)
    {
        nj = strconv::to<size_t>(argv[1],"#jobs");
    }

    if(argc>2)
    {
        nIter = strconv::to<size_t>(argv[2],"nIter");
    }

    engine Q(true);
    wtime  chrono;
    chrono.start();
    // seq
    {
        sequential_dispatcher seq;
        for(size_t i=1;i<=nj;++i)
        {
            Work w(i);
            w(seq[0]);
        }
    }
    const double tseq = chrono.query();

    {
        YOCTO_LOCK(Q.access);
        std::cerr << "\t (*) tseq = " << tseq << std::endl;
        std::cerr << "Enqueue " << nj << " jobs..." << std::endl;
    }
    chrono.start();
    for(size_t i=1;i<=nj;++i)
    {
        Work w(i);
        const job J(w);
        Q.enqueue(J);
    }
    Q.flush();
    const double tpar = chrono.query();
    {
        YOCTO_LOCK(Q.access);
        std::cerr << "\t (*) tseq       = " << tseq      << std::endl;
        std::cerr << "\t (*) tpar       = " << tpar      << std::endl;
        std::cerr << "\t (*) ratio      = " << tseq/tpar << std::endl;
        std::cerr << "\t (*) efficiency = " << (100.0*tseq)/(Q.size*tpar) << "%" << std::endl;
    }

    std::cerr << "create  batch..." << std::endl;
    job_batch jbatch;
    chrono.start();
    for(size_t i=1;i<=nj;++i)
    {
        const Work w(i);
        jbatch.push_back(w);
    }
    std::cerr << "enqueuing batch..." << std::endl;
    Q.enqueue_batch(jbatch);
    std::cerr << "done..." << std::endl;
    Q.flush();
    const double tpar2 = chrono.query();
    {
        YOCTO_LOCK(Q.access);
        std::cerr << "\t (*) tpar2      = " << tpar2      << std::endl;
        std::cerr << "\t (*) ratio      = " << tseq/tpar2 << std::endl;
        std::cerr << "\t (*) efficiency = " << (100.0*tseq)/(Q.size*tpar2) << "%" << std::endl;
    }

    {
        YOCTO_LOCK(Q.access);
        std::cerr << "\t---- Program is quiting ----" << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()
