#include "yocto/threading/scheme/server.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/wtime.hpp"

using namespace yocto;

namespace  {

    class DoSomething
    {
    public:
        explicit DoSomething() throw() {}
        virtual ~DoSomething() throw() {}


        void Run( threading::context &ctx ) throw()
        {
            {
                YOCTO_LOCK(ctx.access);
                std::cerr << "Run on " << ctx.size << "." << ctx.rank << std::endl;
            }
        }

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(DoSomething);
    };

}

YOCTO_UNIT_TEST_IMPL(server)
{
    threading::seq_server seqsrv;
    threading::par_server parsrv(true);

    DoSomething       dummy;
    threading::kernel k( & dummy, & DoSomething::Run );

    
    parsrv.enqueue(k);
    wtime::sleep(1);

}
YOCTO_UNIT_TEST_DONE()
