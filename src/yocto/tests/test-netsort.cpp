#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/sequence/vector.hpp"
#include <typeinfo>

using namespace yocto;

YOCTO_PAIR_DECL(STANDARD,XCH,int,i,int,j);
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


static const XCH xch3[] =
{
    XCH(1,2),
    XCH(0,2),
    XCH(0,1)
};


static const XCH xch4[] =
{
    XCH(0,1), XCH(2,3),
    XCH(0,2), XCH(1,3),
    XCH(1,2)
};


static const XCH xch5[] =
{
    XCH(0,1), XCH(3,4),
    XCH(2,4),
    XCH(2,3), XCH(1,4),
    XCH(0,3),
    XCH(0,2), XCH(1,3),
    XCH(1,2)
};


static const XCH xch6[] =
{
    XCH(1, 2),
    XCH(0, 2),
    XCH(0, 1),
    XCH(4, 5),
    XCH(3, 5),
    XCH(3, 4),
    XCH(0, 3),
    XCH(1, 4),
    XCH(2, 5),
    XCH(2, 4),
    XCH(1, 3),
    XCH(2, 3)
};


static const XCH xch7[] =
{
    XCH(1, 2),
    XCH(0, 2),
    XCH(0, 1),
    XCH(3, 4),
    XCH(5, 6),
    XCH(3, 5),
    XCH(4, 6),
    XCH(4, 5),
    XCH(0, 4),
    XCH(0, 3),
    XCH(1, 5),
    XCH(2, 6),
    XCH(2, 5),
    XCH(1, 3),
    XCH(2, 4),
    XCH(2, 3)
};


static const XCH xch8[] =
{
    XCH(0, 1),
    XCH(2, 3),
    XCH(0, 2),
    XCH(1, 3),
    XCH(1, 2),
    XCH(4, 5),
    XCH(6, 7),
    XCH(4, 6),
    XCH(5, 7),
    XCH(5, 6),
    XCH(0, 4),
    XCH(1, 5),
    XCH(1, 4),
    XCH(2, 6),
    XCH(3, 7),
    XCH(3, 6),
    XCH(2, 4),
    XCH(3, 5),
    XCH(3, 4)
};

static const XCH xch9[] =
{

    XCH(0, 1),

    XCH(2, 3),

    XCH(0, 2),

    XCH(1, 3),

    XCH(1, 2),

    XCH(4, 5),

    XCH(7, 8),

    XCH(6, 8),

    XCH(6, 7),

    XCH(4, 7),

    XCH(4, 6),

    XCH(5, 8),

    XCH(5, 7),

    XCH(5, 6),

    XCH(0, 5),

    XCH(0, 4),

    XCH(1, 6),

    XCH(1, 5),

    XCH(1, 4),

    XCH(2, 7),

    XCH(3, 8),

    XCH(3, 7),

    XCH(2, 5),

    XCH(2, 4),

    XCH(3, 6),

    XCH(3, 5),

    XCH(3, 4)

};

#define __XCH(ID) { ID, sizeof(ID)/sizeof(ID[0]) }
static const XCH_OPS xch_ops[] =
{
    { NULL,0 },
    { NULL,0 },
    __XCH(xch2),
    __XCH(xch3),
    __XCH(xch4),
    __XCH(xch5),
    __XCH(xch6),
    __XCH(xch7),
    __XCH(xch8),
    __XCH(xch9)
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

template <typename T>
inline void check_is_sorted(const array<T> &arr,const char *id)
{
    for(size_t i=2;i<=arr.size();++i)
    {
        if(arr[i-1]>arr[i])
        {
            throw exception("not well sorted '%s'",id);
        }
    }
}

template <typename T,const size_t N>
inline void perform_test()
{
    vector<T> arr(N,as_capacity);
    const string s = vformat("<%s,%d>", typeid(T).name(), int(N) );
    std::cerr << s << std::endl;
    for(size_t iter=0;iter<1000;++iter)
    {
        arr.free();
        for(size_t i=0;i<N;++i)
        {
            arr.push_back( alea.get<T>() );
        }
        __try_sort<T,N>(&arr[1]);
        check_is_sorted(arr,*s);
    }
}


#define __TEST(TYPE) do {\
perform_test<TYPE,2>();\
perform_test<TYPE,3>();\
perform_test<TYPE,4>();\
perform_test<TYPE,5>();\
perform_test<TYPE,6>();\
perform_test<TYPE,7>();\
perform_test<TYPE,8>();\
perform_test<TYPE,9>();\
} while(false)

YOCTO_UNIT_TEST_IMPL(netsort)
{
    __TEST(int);
    __TEST(float);
    __TEST(double);
    __TEST(string);
}
YOCTO_UNIT_TEST_DONE()


