#include "yocto/ipso/subset.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/vtk.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace ipso;

static inline void show_swaps(const swaps &swp)
{
    std::cerr << "\t\t@" << swaps::pos2txt(swp.pos) << " " << swp.source << "<->" << swp.target << std::endl;
    std::cerr << "\t\t|_send=" << swp.send << std::endl;
    std::cerr << "\t\t|_recv=" << swp.recv << std::endl;

}

template <typename COORD>
static inline void show_subs( const subsets<COORD> &subs )
{
    std::cerr << "#subset=" << subs.size << " as " << subs.sizes << std::endl;
    for(const subset<COORD> *sub = subs.head; sub; sub=sub->next )
    {
        std::cerr << "ranks="    << sub->ranks << std::endl;
        std::cerr << "\tinner="  << sub->inner << std::endl;
        std::cerr << "\touter="  << sub->outer << std::endl;
        std::cerr << "\t#local=" << sub->local.size << std::endl;
        for(const swaps *swp = sub->local.head; swp;swp=swp->next)
        {
            show_swaps(*swp);
        }
        std::cerr << "\t#async=" << sub->async.size << std::endl;
        for(const swaps *swp = sub->async.head; swp;swp=swp->next)
        {
            show_swaps(*swp);
        }
    }
}

template <typename COORD>
static inline void show_flags( const subsets<COORD> &subs, const COORD &PBCS)
{
    std::cerr << "flags for sizes=" << subs.sizes << " and PBCS=" << __coord2bool(PBCS) << std::endl;
    for(const subset<COORD> *sub = subs.head; sub; sub=sub->next )
    {
        std::cerr << "\t@" << sub->ranks << std::endl;
        //std::cerr << "\t|_local_flags=" << swaps::flg2str(sub->local_flags) << std::endl;
        //std::cerr << "\t|_async_flags=" << swaps::flg2str(sub->async_flags) << std::endl;
    }
}


#define BULK 5

template <typename FIELD, typename SWAP>
static inline void mark( FIELD &F, const SWAP *sub, const int s, const int r)
{
    std::cerr << "\t\tmarking field" << std::endl;
    for(;sub;sub=sub->next)
    {
        F.swap_ops(sub->send,  s,  field_set);
        F.swap_ops(sub->recv,  r,  field_set);
    }
}

YOCTO_UNIT_TEST_IMPL(subset)
{
    if(false)
        for(size_t dim=0;dim<3;++dim)
        {
            std::cerr << "flag " << dim  << " -1 = " << swaps::dim2pos(dim,-1) << std::endl;
            std::cerr << "flag " << dim  << "  1 = " << swaps::dim2pos(dim,1)  << std::endl;
        }
    assert( swaps::lower_x == swaps::dim2pos(0, -1) );
    assert( swaps::upper_x == swaps::dim2pos(0,  1) );
    assert( swaps::lower_y == swaps::dim2pos(1, -1) );
    assert( swaps::upper_y == swaps::dim2pos(1,  1) );
    assert( swaps::lower_z == swaps::dim2pos(2, -1) );
    assert( swaps::upper_z == swaps::dim2pos(2,  1) );


    if(argc<=4)
    {
        throw exception("usage: %s DIMS PBCS CPUS LAYERS", argv[0]);
    }
    const coord3D dims   = __coord_parser::get<coord3D>(argv[1],"dims");
    const coord3D pbcs   = __coord_parser::get<coord3D>(argv[2],"pbcs");
    const size_t  cpus   = strconv::to_size(argv[3],"cpus");
    const size_t  layers = strconv::to_size(argv[4],"layers");

    std::cerr << "dims   = " << dims   << std::endl;
    std::cerr << "pbcs   = " << pbcs   << std::endl;
    std::cerr << "cpus   = " << cpus   << std::endl;
    std::cerr << "layers = " << layers << std::endl;

    if(true)
    {
        std::cerr << std::endl << "-------- 1D --------" << std::endl;
        const patch1D      region(1,dims.x);
        const coord1D      PBCS(pbcs.x);
        coord1D            fallback=0;
        const coord1D sizes = divider<coord1D>::optimal_for(region,cpus,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in1D full(sizes,region);
        subsets<coord1D>   subs(full,layers,PBCS,true);
        show_subs(subs);
        for(const subset<coord1D> *sub = subs.head; sub; sub=sub->next )
        {

            field1D<float> Field("coms",sub->outer);
            Field.ldz();
            Field.ld_on(sub->inner,BULK);
            mark(Field,sub->local.head,1,2);
            mark(Field,sub->async.head,3,4);

            const string  fn = vformat("f1d_%u.vtk",unsigned(sub->rank));
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "in1D", Field, sub->outer);
        }
        show_flags(subs,PBCS);
    }

    if(true)
    {
        std::cerr << std::endl << "-------- 2D --------" << std::endl;
        const patch2D      region(coord2D(1,1),dims.xy());
        const coord2D      PBCS(pbcs.xy());
        coord2D            fallback;
        const coord2D sizes = divider<coord2D>::optimal_for(region,cpus,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in2D full(sizes,region);
        subsets<coord2D>   subs(full,layers,PBCS,true);
        show_subs(subs);

        for(const subset<coord2D> *sub = subs.head; sub; sub=sub->next )
        {
            field2D<float> Field("coms",sub->outer);
            Field.ldz();
            Field.ld_on(sub->inner,BULK);
            mark(Field,sub->local.head,1,2);
            mark(Field,sub->async.head,3,4);

            const string  fn = vformat("f2d_%u.vtk",unsigned(sub->rank));
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "in2D", Field, sub->outer);
        }
        show_flags(subs,PBCS);
    }

    if(false)
    {
        std::cerr << std::endl << "-------- 3D --------" << std::endl;
        const patch3D      region(coord3D(1,1,1),dims);
        const coord3D      PBCS(pbcs);
        coord3D            fallback;
        const coord3D sizes = divider<coord3D>::optimal_for(region,cpus,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in3D full(sizes,region);
        subsets<coord3D>   subs(full,layers,PBCS,true);
        show_subs(subs);

        for(const subset<coord3D> *sub = subs.head; sub; sub=sub->next )
        {
            field3D<float> Field("coms",sub->outer);
            Field.ldz();
            Field.ld_on(sub->inner,BULK);
            mark(Field,sub->local.head,1,2);
            mark(Field,sub->async.head,3,4);

            const string  fn = vformat("f3d_%u.vtk",unsigned(sub->rank));
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "in3D", Field, sub->outer);
        }
    }




}
YOCTO_UNIT_TEST_DONE()

