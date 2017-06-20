#include "yocto/mpi/mpi.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/hashing/sha1.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(sr)
{
    YOCTO_MPI_ENV();

    size_t n = 10;
    hashing::sha1 H;

    typedef uint16_t word_t;
    typedef unsigned hash_t;
    const int rank = MPI.CommWorldRank;


    //MPI.CloseStdIO();
    if(MPI.CommWorldSize>0)
    {
        MPI.Printf(stderr, "Starting Send/Recv...\n");
        vector<word_t> sndbuf(n);
        vector<word_t> rcvbuf(n);
        for(size_t i=1;i<=n;++i)
        {
            sndbuf[i] = _rand.full<word_t>();
        }
        if(1==MPI.CommWorldSize)
        {
            std::cerr << "sndbuf=" << sndbuf << std::endl;
        }

        const hash_t SH = H.key<hash_t>( &sndbuf[1], n*sizeof(word_t) );
        MPI.Printf(stderr,"SH%d=%08x\n",rank,SH);

    }
    else
    {
        MPI.Printf0(stderr,"Not Parallel...\n");
    }
}
YOCTO_UNIT_TEST_DONE()
