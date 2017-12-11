#include "yocto/sort/nwsrt.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/comparator.hpp"
#include "yocto/string.hpp"

using namespace yocto;


template <typename SEQ>
void __fill_seq( SEQ &a, const size_t N )
{
    a.free();
    for(size_t i=0;i<N;++i)
    {
        typename SEQ::type tmp = alea.get<typename SEQ::mutable_type>();
        a.push_back(tmp);
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


#define __EXE(TYPE,N) do {\
for(size_t iter=0;iter<1000;++iter)\
{\
__fill_seq(arr,N); assert(N==arr.size());\
nwsrt<TYPE>::op##N( &arr[1], __compare<TYPE> );\
check_is_sorted(arr,#N);\
}\
} while(false)

#define __RUN(TYPE) do {\
vector<TYPE> arr(32,as_capacity);\
__EXE(TYPE,2);\
__EXE(TYPE,3);\
__EXE(TYPE,4);\
__EXE(TYPE,5);\
__EXE(TYPE,6);\
__EXE(TYPE,7);\
__EXE(TYPE,8);\
__EXE(TYPE,9);\
__EXE(TYPE,10);\
__EXE(TYPE,11);\
__EXE(TYPE,12);\
__EXE(TYPE,13);\
__EXE(TYPE,14);\
__EXE(TYPE,15);\
__EXE(TYPE,16);\
__EXE(TYPE,17);\
__EXE(TYPE,18);\
__EXE(TYPE,19);\
__EXE(TYPE,20);\
__EXE(TYPE,21);\
__EXE(TYPE,22);\
__EXE(TYPE,23);\
__EXE(TYPE,24);\
__EXE(TYPE,25);\
__EXE(TYPE,26);\
__EXE(TYPE,27);\
__EXE(TYPE,28);\
__EXE(TYPE,29);\
__EXE(TYPE,30);\
__EXE(TYPE,31);\
__EXE(TYPE,32);\
}while(false)


YOCTO_UNIT_TEST_IMPL(nwsrt)
{
    __RUN(int);
    __RUN(float);
    __RUN(double);
    __RUN(string);
}
YOCTO_UNIT_TEST_DONE()
