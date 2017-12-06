#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/sequence/vector.hpp"

using namespace yocto;

YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,XCH,int,i,int,j);
YOCTO_PAIR_END();

struct XCH_OPS
{
    const XCH *xchs;
    size_t     nxch;
};

static const XCH xch2[] =
{
    XCH(0,1)
};

//static const int par2[1] = {0};

static const XCH xch3[] =
{
    XCH(1,2),
    XCH(0,2),
    XCH(0,1)
};

/*
 [[0,1],[2,3]]
 [[0,2],[1,3]]
 [[1,2]]
 */
static const XCH xch4[] =
{
    XCH(0,1), XCH(2,3),
    XCH(0,2), XCH(1,3),
    XCH(1,2)
};

/*
[[0,1],[3,4]]
[[2,4]]
[[2,3],[1,4]]
[[0,3]]
[[0,2],[1,3]]
[[1,2]]
*/

static const XCH xch5[] =
{
    XCH(0,1), XCH(3,4),
    XCH(2,4),
    XCH(2,3), XCH(1,4),
    XCH(0,3),
    XCH(0,2), XCH(1,3),
    XCH(1,2)
};

#define __XCH(ID) { ID, sizeof(ID)/sizeof(ID[0]) }
static const XCH_OPS xch_ops[] =
{
    { NULL,0 },
    { NULL,0 },
    __XCH(xch2),
    __XCH(xch3),
    __XCH(xch4),
    __XCH(xch5)
};

template <typename T,const size_t N>
inline void __try_sort(T *arr) throw()
{
    const XCH_OPS *ops  = &xch_ops[N];
    const XCH     *xchs = ops->xchs;
    const size_t   nxch = ops->nxch;
    for(size_t k=0;k<nxch;++k)
    {
        const int I = xchs[k].i;
        const int J = xchs[k].j;
        if(arr[J]<arr[I])
        {
            bswap(arr[J],arr[I]);
        }
    }
}

template <typename T,const size_t N>
inline void perform_test()
{
    vector<T> arr(N,as_capacity);
    for(size_t i=0;i<N;++i)
    {
        arr.push_back( alea.get<T>() );
    }
    for(size_t iter=0;iter<1;++iter)
    {
        alea.shuffle(&arr[1],N);
        __try_sort<T,N>(&arr[1]);
    }
}




YOCTO_UNIT_TEST_IMPL(netsort)
{
    perform_test<int,2>();
}
YOCTO_UNIT_TEST_DONE()


