#include "yocto/threading/scheme/server.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(server)
{
    threading::seq_server seqsrv;
    threading::par_server parsrv(true);

}
YOCTO_UNIT_TEST_DONE()
