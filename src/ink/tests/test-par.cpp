#include "yocto/ink/parallel.hpp"
#include "yocto/ipso/coord.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/threading/scheme/server.hpp"

using namespace yocto;
using namespace Ink;

namespace
{

    class DoSomething
    {
    public:
        inline explicit DoSomething() throw() {}
        inline virtual ~DoSomething() throw() {}

        void run(const Domain &rect, threading::context &ctx )
        {
            YOCTO_LOCK(ctx.access);
            std::cerr << "in context " << ctx.size << "." << ctx.rank << std::endl;
            std::cerr << "in domain  " << rect.ranks << std::endl;
        }

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(DoSomething);
    };
}

YOCTO_UNIT_TEST_IMPL(par)
{

    SharedServer seq( new threading::seq_server()     );
    SharedServer par( new threading::par_server(true) );

    std::cerr << "seq.threads=" << seq->cpu.num_threads() << std::endl;
    std::cerr << "par.threads=" << par->cpu.num_threads() << std::endl;

    if(argc<=1) throw exception("usage: %s W,H",argv[0]);
    const coord  area = ipso::__coord_parser::get<coord>(argv[1],"area");
    Area         full(0,0,area.x,area.y);

    const coord  seq_sizes = Domains::GetPartitionFor(full,seq);
    const coord  par_sizes = Domains::GetPartitionFor(full,par);
    std::cerr << "seq_size=" << seq_sizes << std::endl;
    std::cerr << "par_size=" << par_sizes << std::endl;

    Domains seq_doms(full,seq);
    Domains par_doms(full,par);

    

    DoSomething host;
    seq_doms.submit(&host, & DoSomething::run );
    par_doms.submit(&host, & DoSomething::run );
}
YOCTO_UNIT_TEST_DONE()

