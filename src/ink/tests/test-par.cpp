#include "yocto/ink/parallel.hpp"
#include "yocto/ipso/coord.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/threading/scheme/server.hpp"

using namespace yocto;
using namespace Ink;

YOCTO_UNIT_TEST_IMPL(par)
{
    threading::seq_server seq;
    threading::par_server par;
    std::cerr << "seq.threads=" << seq.num_threads() << std::endl;
    std::cerr << "par.threads=" << par.num_threads() << std::endl;

    if(argc<=1) throw exception("usage: %s W,H",argv[0]);
    const coord  area = ipso::__coord_parser::get<coord>(argv[1],"area");
    Rectangle    full(0,0,area.x,area.y);


    const coord  seq_sizes = Partition::Optimal(full,seq.num_threads());
    const coord  par_sizes = Partition::Optimal(full,par.num_threads());
    std::cerr << "seq_size=" << seq_sizes << std::endl;
    std::cerr << "par_size=" << par_sizes << std::endl;
    
#if 0
    if(argc<=2) throw exception("usage: %s W,H #CORE",argv[0]);

    const coord  area = ipso::__coord_parser::get<coord>(argv[1],"area");
    const size_t cores = strconv::to_size(argv[2],"cores");

    Rectangle full(0,0,area.x,area.y);
    Partition::List parts;
    Partition::Build(parts,full,cores);
    std::cerr << "for area=" << area << " and #core=" << cores << std::endl;
    for(const Partition *p=parts.head;p;p=p->next)
    {
        std::cerr << p->sizes << " \t score=" << p->score << std::endl;
    }

    threading::seq_server seq;
    threading::par_server par;
    std::cerr << "par_server.size=" << par.size << std::endl;
#endif

}
YOCTO_UNIT_TEST_DONE()

