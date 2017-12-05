#include "yocto/utest/run.hpp"
#include "yocto/ipso/split.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/string/tokenizer.hpp"
#include "yocto/string.hpp"
#include "yocto/string/conv.hpp"

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

YOCTO_UNIT_TEST_IMPL(ipso_split)
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

    const patch2D p2( coord2D(1,1), coord2D(dims.x,dims.y) );
    const coord2D pbc2( pbcs.x, pbcs.y );
    const coord2D nc2 = split::in2D::computeCoresMap(cpus,p2.width,pbc2);
    std::cerr << "ncore2D=" << nc2 << std::endl;

    const patch3D p3( coord3D(1,1,1), dims);
    const coord3D nc3 = split::in3D::computeCoresMap(cpus,p3.width,pbcs);
    std::cerr << "ncore3D=" << nc3 << std::endl;


}
YOCTO_UNIT_TEST_DONE()

