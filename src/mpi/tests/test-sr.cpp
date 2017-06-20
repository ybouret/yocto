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


    MPI.CloseStdIO();
    if(MPI.CommWorldSize>1)
    {
        MPI.Printf0(stderr, "Starting Send/Recv...\n");
        vector<word_t> sndbuf(n);
        vector<word_t> rcvbuf(n);

        word_t       *sptr = &sndbuf[1];
        word_t       *rptr = &rcvbuf[1];
        const size_t  xnum = n*sizeof(word_t);

        for(size_t i=1;i<=n;++i)
        {
            sndbuf[i] = _rand.full<word_t>();
        }
        if(1==MPI.CommWorldSize)
        {
            std::cerr << "sndbuf=" << sndbuf << std::endl;
        }

        const hash_t SH0 = H.key<hash_t>(sndbuf);
        const hash_t RH0 = H.key<hash_t>(rcvbuf);
        MPI.Printf(stderr,"SH0=%08X | RH0=%08X\n",SH0,RH0);

        const int next = MPI.CommWorldNext();
        const int prev = MPI.CommWorldPrev();
        MPI.Printf(stderr," %d->%d->%d\n", prev, rank, next);

        MPI_Status status;
        MPI.Sendrecv(sptr, xnum, MPI_BYTE, next, 0,
                     rptr, xnum, MPI_BYTE, prev, 0,
                     MPI_COMM_WORLD,
                     status);

        const hash_t SH1 = H.key<hash_t>(sndbuf);
        const hash_t RH1 = H.key<hash_t>(rptr,xnum);
        MPI.Printf(stderr,"SH1=%08X | RH1=%08X\n",SH1,RH1);


    }
    else
    {
        MPI.Printf0(stderr,"Not Parallel...\n");
    }
}
YOCTO_UNIT_TEST_DONE()
