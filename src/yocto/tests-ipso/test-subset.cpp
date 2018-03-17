#include "yocto/ipso/subset.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ipso/vtk.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;
using namespace ipso;

static inline void show_swaps(const swaps &swp)
{
    const string where = swaps::flg2str(swp.pos);
    std::cerr << "\t\t@" << where << " " << swp.source << "->" << swp.target << std::endl;
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
        std::cerr << "\t#asyncs=" << sub->asyncs.size << std::endl;
        for(size_t dim=0;dim<YOCTO_IPSO_DIM_OF(COORD);++dim)
        {
            std::cerr << "-- Dimension " << dim << std::endl;
            for(const swaps *swp = sub->async[dim].head; swp; swp=swp->next)
            {
                show_swaps(*swp);
            }
        }

        std::cerr << std::endl;
    }
}



#define BULK 5

template <typename FIELD, typename SWAP>
static inline void mark( FIELD &F, const SWAP *sub, const int s, const int r)
{
    //std::cerr << "\t\tmarking field" << std::endl;
    for(;sub;sub=sub->next)
    {
        F.swap_ops(sub->send, typename FIELD::type(s),  field_set);
        F.swap_ops(sub->recv, typename FIELD::type(r),  field_set);
    }
}

template <typename SUBSET>
static inline void check_io( SUBSET *sub, fields &fvar )
{
    std::cerr << "\t** allocating for block_size=" << fvar.block_size() << std::endl;
    sub->allocate_swaps_for(fvar);

    std::cerr << "\t** field by field I/O" << std::endl;
    sub->sync_store_begin();
    for(size_t i=fvar.size();i>0;--i)
    {
        sub->sync_store(*fvar[i]);
    }
    sub->sync_store_end();

    for(size_t i=fvar.size();i>0;--i)
    {
        sub->sync_query( *fvar[i] );
    }

    std::cerr << "\t** fields I/O" << std::endl;
    sub->sync_start(fvar);
    sub->sync_query(fvar);
    
}

YOCTO_UNIT_TEST_IMPL(subset)
{
    if(true)
    {
        for(size_t dim=0;dim<3;++dim)
        {
            std::cerr << "flag " << dim  << " -1 = " << swaps::dim2pos(dim,-1) << std::endl;
            std::cerr << "flag " << dim  << "  1 = " << swaps::dim2pos(dim,1)  << std::endl;
        }
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

    field_db DB;
    fields   fvar;

    if(true)
    {
        std::cerr << std::endl << "-------- 1D --------" << std::endl;
        const patch1D      region(1,dims.x);
        const coord1D      PBCS(pbcs.x);
        coord1D            fallback=0;
        const coord1D sizes = mapping<coord1D>::optimal_sizes_for(cpus,region,layers,PBCS,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in1D full(sizes,region);
        subsets<coord1D>   subs(full,layers,PBCS,true);
        show_subs(subs);
        for( subset<coord1D> *sub = subs.head; sub; sub=sub->next )
        {
            DB.free();
            fvar.free();
            field1D<float>  &F1 = DB.build< field1D<float>  >("f1",sub->outer);
            field1D<double> &F2 = DB.build< field1D<double> >("f2",sub->outer);
            fvar.append(F1);
            fvar.append(F2);
            F1.ldz();
            F1.ld_on(sub->inner,BULK);
            mark(F1,sub->local.head,1,2);
            mark(F1,sub->async[0].head,3,4);

            const string  fn = vformat("f1d_%u.vtk",unsigned(sub->rank));
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "in1D", F1, sub->outer);

            check_io(sub,fvar);
        }
    }
    return 0;
    
    if(true)
    {

        std::cerr << std::endl << "-------- 2D --------" << std::endl;
        const patch2D      region(coord2D(1,1),dims.xy());
        const coord2D      PBCS(pbcs.xy());
        coord2D            fallback;
        const coord2D sizes = mapping<coord2D>::optimal_sizes_for(cpus,region,layers,PBCS,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in2D full(sizes,region);
        subsets<coord2D>   subs(full,layers,PBCS,true);
        show_subs(subs);

        for( subset<coord2D> *sub = subs.head; sub; sub=sub->next )
        {
            DB.free();
            fvar.free();
            field2D<float>             &F1 = DB.build< field2D<float> >("f1",sub->outer);
            field2D< point2d<double> > &F2 = DB.build< field2D<point2d<double> > >("f2",sub->outer);
            fvar.append(F1);
            fvar.append(F2);

            F1.ldz();
            F1.ld_on(sub->inner,BULK);
            mark(F1,sub->local.head,1,2);
            mark(F1,sub->async[0].head,3,4);
            mark(F1,sub->async[1].head,3,4);

            const string  fn = vformat("f2d_%u.vtk",unsigned(sub->rank));
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "in2D", F1, sub->outer);

            check_io(sub,fvar);

        }
    }

    
    if(true)
    {
        std::cerr << std::endl << "-------- 3D --------" << std::endl;
        const patch3D      region(coord3D(1,1,1),dims);
        const coord3D      PBCS(pbcs);
        coord3D            fallback;
        const coord3D sizes = mapping<coord3D>::optimal_sizes_for(cpus,region,layers,PBCS,&fallback);
        std::cerr << "sizes=" << sizes << " | fallback=" << fallback << std::endl;
        const divide::in3D full(sizes,region);
        subsets<coord3D>   subs(full,layers,PBCS,true);
        show_subs(subs);

        for( subset<coord3D> *sub = subs.head; sub; sub=sub->next )
        {
            DB.free();
            fvar.free();

            field3D<float> &F1 = DB.build< field3D<float> >("f1",sub->outer);
            field3D<short> &F2 = DB.build< field3D<short> >("f2",sub->outer);
            fvar.append(F1);
            fvar.append(F2);

            F1.ldz();
            F1.ld_on(sub->inner,BULK);
            mark(F1,sub->local.head,1,2);
            mark(F1,sub->async[0].head,3,4);
            mark(F1,sub->async[1].head,3,4);
            mark(F1,sub->async[2].head,3,4);

            const string  fn = vformat("f3d_%u.vtk",unsigned(sub->rank));
            ios::wcstream fp(fn);
            VTK::InitSaveScalars(fp, "in3D", F1, sub->outer);

            check_io(sub,fvar);

        }
    }




}
YOCTO_UNIT_TEST_DONE()


