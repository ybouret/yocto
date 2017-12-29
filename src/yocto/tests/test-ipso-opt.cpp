
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

static size_t ng = 2;

YOCTO_UNIT_TEST_IMPL(ipso_opt)
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

}
YOCTO_UNIT_TEST_DONE()

