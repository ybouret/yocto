#include "yocto/utest/run.hpp"
#include "yocto/ipso/partition.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/string/tokenizer.hpp"
#include "yocto/ipso/field3d.hpp"
#include "yocto/ptr/arc.hpp"

#include <cstdio>

using namespace yocto;
using namespace ipso;


static inline bool is_sep(char C) throw() { return ',' == C; }
static inline bool is_bad(char C) throw() { return ' ' == C; }

static inline
coord3D parse_triple(const char *text,const char *name)
{
    static const char *id[4] = { "", "x", "y", "z" };

    coord3D        ans;
    vector<string> words;
    const string   data = text;
    tokenizer::split(words,data,is_sep);
    if(words.size()!=3)
    {
        throw exception("%s: wrong size", name);
    }

    coord1D *q = ( (coord1D *)&ans )  - 1;
    for(size_t i=1;i<=words.size();++i)
    {
        words[i].clean(is_bad);
        q[i] = strconv::to_int(words[i],id[i]);
    }

    return ans;
}

static size_t ng = 1;

typedef double data_t;
typedef field1D<data_t>  F1D;
typedef arc_ptr<F1D>     F1Dptr;

static inline
void test1D( const coord3D dims, const coord3D pbcs, const size_t cpus)
{
    std::cerr << "1D" << std::endl;
    const patch1D      region(1,dims.x);
    const divide::in1D full(cpus,region);
    const partition1D  parts(full,ng,pbcs.x,true);
    vector<F1Dptr>     fields;
    std::cerr << "Fields: " << std::endl;
    size_t f = 0;
    for(const domain1D *d = parts.head; d; d=d->next )
    {
        ++f;
        std::cerr << "field#" << f << std::endl;
        std::cerr << "\tinner: " << d->inner << std::endl;
        std::cerr << "\touter: " << d->outer << std::endl;
        F1Dptr pF( new F1D(d->outer) );
        fields.push_back(pF);
        pF->ld(1);
    }
}

YOCTO_UNIT_TEST_IMPL(ipso_coms)
{
    if(argc<=3)
    {
        throw exception("usage: %s DIMS PBCS CPUS", argv[0]);
    }
    const coord3D dims = parse_triple(argv[1],"dims");
    const coord3D pbcs = parse_triple(argv[2],"pbcs");
    const size_t  cpus = strconv::to_size(argv[3],"cpus");
    std::cerr << "dims=" << dims << std::endl;
    std::cerr << "pbcs=" << pbcs << std::endl;
    std::cerr << "cpus=" << cpus << std::endl;

    test1D(dims,pbcs,cpus);

}
YOCTO_UNIT_TEST_DONE()

