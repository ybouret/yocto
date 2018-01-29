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
    threading::seq_server seq;
    threading::par_server par(true);
    std::cerr << "seq.threads=" << seq.num_threads() << std::endl;
    std::cerr << "par.threads=" << par.num_threads() << std::endl;

    if(argc<=1) throw exception("usage: %s W,H",argv[0]);
    const coord  area = ipso::__coord_parser::get<coord>(argv[1],"area");
    Area         full(0,0,area.x,area.y);


    const coord  seq_sizes = Partition::Optimal(full,seq.num_threads());
    const coord  par_sizes = Partition::Optimal(full,par.num_threads());
    std::cerr << "seq_size=" << seq_sizes << std::endl;
    std::cerr << "par_size=" << par_sizes << std::endl;
    const Partition seq_part(seq_sizes,full);
    const Partition par_part(par_sizes,full);
    Domains seq_doms;
    Domains par_doms;
    seq_part.compute(&seq_doms);
    par_part.compute(&par_doms);
    std::cerr << "#seq_doms=" << seq_doms.size << "\t@score=" << seq_part.score << std::endl;
    std::cerr << "#par_doms=" << par_doms.size << "\t@score=" << par_part.score << std::endl;

    DoSomething host;
    seq_doms.enqueue(seq, &host, & DoSomething::run );
    seq.flush();
    par_doms.enqueue(par, &host, & DoSomething::run );
    par.flush();

}
YOCTO_UNIT_TEST_DONE()

