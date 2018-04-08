
#include "yocto/ipso/mapping.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/format/vtk.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace ipso;

YOCTO_UNIT_TEST_IMPL(onesub)
{
    if(argc<=4)
    {
        throw exception("usage: %s DIMS PBCS CPUS LAYERS", argv[0]);
    }

    const coord2D dims   = __coord_parser::get<coord2D>(argv[1],"dims");
    const coord2D pbcs   = __coord_parser::get<coord2D>(argv[2],"pbcs");
    const coord2D cpus   = __coord_parser::get<coord2D>(argv[3],"cpus");
    const size_t  layers = strconv::to_size(argv[4],"layers");

    std::cerr << "dims   = " << dims   << std::endl;
    std::cerr << "pbcs   = " << pbcs   << std::endl;
    std::cerr << "cpus   = " << cpus   << std::endl;
    std::cerr << "layers = " << layers << std::endl;

    //const size_t ncpu = __coord_prod(cpus);

    if(true)
    {
        const patch2D      region(coord2D(1,1),dims);
        const divide::in2D full(cpus,region);
        subsets<coord2D>   subs(full,layers,pbcs,true);

        for(const subset<coord2D> *sub = subs.head; sub; sub=sub->next )
        {

            field2D<double> f("f",sub->outer);
            f.ldz();
            for(const swaps *swp =  sub->cross.head; swp; swp=swp->next)
            {
                const double value = (swp->target)+1;
                for(size_t i=swp->count;i>0;--i)
                {
                    const coord1D iSend = swp->send[i];
                    const coord1D iRecv = swp->recv[i];
                    f.entry[iSend] =  value;
                    f.entry[iRecv] = -value;
                }
            }

            {
                const string  fn = "field" + vformat("%u",unsigned(sub->rank)) + ".vtk";
                ios::wcstream fp(fn);
                VTK::InitSaveScalars(fp, "cross", f, f);
            }
        }

    }

}
YOCTO_UNIT_TEST_DONE()
