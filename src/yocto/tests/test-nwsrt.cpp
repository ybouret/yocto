#include "yocto/sort/nwsrt.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"

using namespace yocto;


template <typename SEQ>
void __fill_seq( SEQ &a, const size_t N )
{
    a.free();
    for(size_t i=0;i<N;++i)
    {
        const T tmp = alea.gen<typename SEQ::mutable_type>();
        a.push_back(T);
    }
}

YOCTO_UNIT_TEST_IMPL(nwsrt)
{
}
YOCTO_UNIT_TEST_DONE()
