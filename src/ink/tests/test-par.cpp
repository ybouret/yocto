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

    Engine::SharedServer seq( new threading::seq_server()     );
    Engine::SharedServer par( new threading::par_server(true) );

    std::cerr << "seq.threads=" << seq->cpu.num_threads() << std::endl;
    std::cerr << "par.threads=" << par->cpu.num_threads() << std::endl;

    if(argc<=1) throw exception("usage: %s W,H",argv[0]);
    const coord  area = ipso::__coord_parser::get<coord>(argv[1],"area");
    Area         full(0,0,area.x,area.y);

    Engine seq_doms(full,seq);
    return 0;
    
    Engine par_doms(full,par);




    DoSomething host;
    seq_doms.submit(&host, & DoSomething::run );
    par_doms.submit(&host, & DoSomething::run );
}
YOCTO_UNIT_TEST_DONE()

