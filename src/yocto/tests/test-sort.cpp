#include "yocto/utest/run.hpp"
#include "yocto/sort/heap.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/comparator.hpp"
#include "yocto/sort/ysort.hpp"
#include "yocto/sort/summation.hpp"

#include "support.hpp"

using namespace yocto;

template <typename T>
static inline void check_sorted( const array<T> &ra, const char *ctx)
{
    assert(ctx);
    for(unsigned j=2;j<=ra.size();++j)
    {
        if(ra[j]<ra[j-1]) throw exception("%s: error at indices %u/%u", ctx, j-1,j);
    }
}


template <typename T>
static inline void test_sort()
{
    std::cerr << "-- sorting" << std::endl;
    const size_t n = 1 + alea.leq( 20 );
    vector<T>    ra(n,as_capacity);
    for( size_t i=0; i < n; ++i )
    {
        T tmp( gen<T>::get() );
        ra.push_back( tmp );
    }
    std::cerr << "raw=" << ra << std::endl;
    hsort( ra, __compare<T> );
    std::cerr << "srt=" << ra << std::endl;
    check_sorted(ra, "sorting");
}

template <typename T,typename U>
static inline void test_cosort()
{
    std::cerr << "-- co-sorting" << std::endl;
    const size_t n = 1 + alea.leq( 20 );
    vector<T>    ra(n,as_capacity);
    vector<U>    rb(n,as_capacity);
    for( size_t i=0; i < n; ++i )
    {
        {
            T tmp( gen<T>::get() );
            ra.push_back( tmp );
        }
        {
            U tmp( gen<U>::get() );
            rb.push_back( tmp );
        }
    }
    std::cerr << "rawa=" << ra << std::endl;
    std::cerr << "rawb=" << rb << std::endl;
    hsort( ra, rb, __compare<T> );
    std::cerr << "srta=" << ra << std::endl;
    std::cerr << "srtb=" << rb << std::endl;
    check_sorted(ra, "co-sorting");
}




template <typename T>
static inline void test_sum()
{
    const size_t N = 10 + alea.leq(10);
    vector<T> data(N+5+alea.leq(5));
    for(size_t i=N;i>0;--i)
    {
        data[i] = alea.get<T>();
    }

    alea.shuffle(&data[1],N);
    const T sum = summation(&data[1],N);
    std::cerr << "(1/2) data=" << data << std::endl;
    std::cerr << "(1/2) sum =" << sum << std::endl;
    alea.shuffle(&data[1],N);
    const T sumb = summation(data,N);
    std::cerr << "(2/2) data=" << data << std::endl;
    std::cerr << "(2/2) sum =" << sumb << std::endl;

}

YOCTO_UNIT_TEST_IMPL(sort)
{
    test_sort<int>();
    test_sort<float>();
    test_sort<string>();
    test_cosort<int,string>();
    

    test_sum<int>();
    test_sum<float>();
    test_sum<double>();
    
}
YOCTO_UNIT_TEST_DONE()

#include "yocto/sort/quick.hpp"
template <typename T>
static inline void test_qsort()
{
    std::cerr << "-- sorting" << std::endl;
    const size_t n = 1 + alea.leq( 20 );
    vector<T>    ra(n,as_capacity);
    for( size_t i=0; i < n; ++i )
    {
        T tmp( gen<T>::get() );
        ra.push_back( tmp );
    }
    std::cerr << "raw=" << ra << std::endl;
    quicksort( ra, __compare<T> );
    std::cerr << "srt=" << ra << std::endl;
    check_sorted(ra, "sorting");
}

template <typename T,typename U>
static inline void test_coqsort()
{
    std::cerr << "-- co-quicksorting" << std::endl;
    const size_t n = 1 + alea.leq( 20 );
    vector<T>    ra(n,as_capacity);
    vector<U>    rb(n,as_capacity);
    for( size_t i=0; i < n; ++i )
    {
        {
            T tmp( gen<T>::get() );
            ra.push_back( tmp );
        }
        {
            U tmp( gen<U>::get() );
            rb.push_back( tmp );
        }
    }
    std::cerr << "rawa=" << ra << std::endl;
    std::cerr << "rawb=" << rb << std::endl;
    co_qsort( ra, rb, __compare<T> );
    std::cerr << "srta=" << ra << std::endl;
    std::cerr << "srtb=" << rb << std::endl;
    check_sorted(ra, "co-sorting");
}


YOCTO_UNIT_TEST_IMPL(qsort)
{
    test_qsort<int>();
    test_qsort<float>();
    test_qsort<double>();
    test_qsort<string>();
    test_coqsort<string,int>();
    test_coqsort<int,string>();
}
YOCTO_UNIT_TEST_DONE()

#include "yocto/sys/wtime.hpp"

#if defined(NDEBUG)
#define ITER_MAX 2048
#else
#define ITER_MAX 1024
#endif

#include <cstdlib>

enum SortWith
{
    HeapSort,
    QuickSort,
    LibcSort,
    YoctoSort
};

template <typename T>
inline int compare_args( const void *lhs, const void *rhs ) throw()
{
    return __compare( *(const T*)lhs, *(const T*)rhs);
}

template <typename T>
inline double test_perf( size_t n, SortWith proc )
{
    wtime chrono;
    chrono.start();
    
    vector<T> arr(n,as_capacity);
    for(size_t i=0;i<n;++i) { const T tmp( gen<T>::get() ); arr.push_back(tmp); }
    double average = 0;
    for(size_t i=0;i<ITER_MAX;++i)
    {
        alea.shuffle( &arr[1], n);
        const double stamp = chrono.query();
        switch(proc)
        {
            case HeapSort:
                hsort(arr);
                break;
                
            case QuickSort:
                quicksort(arr);
                break;
                
            case LibcSort:
                qsort(&arr[1],n,sizeof(T),compare_args<T>);
                break;

            case YoctoSort:
                _ySort(&arr[1],__compare<T>, 0, n-1);
        }
        average += chrono.query() - stamp;
        check_sorted(arr,"sorting perf");
    }
    return average/ITER_MAX;
}

#include "yocto/ios/ocstream.hpp"

template <typename T>
inline void compare_perf(size_t nmax, const char *filename )
{
    std::cerr << std::endl << filename << std::endl;
    ios::ocstream::overwrite(filename);
    for(size_t n=4;n<=nmax;n += 4)
    {
        (std::cerr << ".").flush();
        const double htmx = test_perf<T>(n,HeapSort);
        const double qtmx = test_perf<T>(n,QuickSort);
        const double ctmx = test_perf<T>(n,LibcSort);
        const double ytmx = test_perf<T>(n,YoctoSort);
        ios::ocstream fp(filename,true);
        fp("%g %g %g %g %g\n", double(n), htmx, qtmx, ctmx, ytmx);
    }
    std::cerr << std::endl;
    
}


YOCTO_UNIT_TEST_IMPL(sort_perf)
{
    const size_t nmax = 256;
    compare_perf<int>(nmax,"perf_int.dat");
    compare_perf<float>(nmax,"perf_flt.dat");
    compare_perf<double>(nmax,"perf_dbl.dat");
    compare_perf<string>(nmax,"perf_str.dat");

}
YOCTO_UNIT_TEST_DONE()

template <typename T>
static inline void test_ysort()
{
    std::cerr << "-- sorting/ySort" << std::endl;
    for(size_t iter=0;iter<1000;++iter)
    {
        const size_t n = 1 + alea.leq( 5000 );
        vector<T>    ra(n,as_capacity);
        for( size_t i=0; i < n; ++i )
        {
            T tmp( gen<T>::get() );
            ra.push_back( tmp );
        }
        //std::cerr << "raw=" << ra << std::endl;
        ySort( ra, __compare<T> );
        //std::cerr << "srt=" << ra << std::endl;
        check_sorted(ra, "ySort");

        alea.shuffle(&ra[1],n);
        vector<int> indx(n);
        for(size_t i=1;i<=n;++i)
        {
            indx[i]=i;
        }
        yCoSort(ra,indx,__compare<T>);
        check_sorted(ra,"yCoSort/values");
        yCoSort(indx,ra,__compare<int>);
        check_sorted(indx,"yCoSort/indices");
    }
}

#define __SHOW_IVEC std::cerr << ivec << std::endl
YOCTO_UNIT_TEST_IMPL(ysort)
{
    test_ysort<int>();
    test_ysort<float>();
    test_ysort<double>();
    test_ysort<string>();
}
YOCTO_UNIT_TEST_DONE()

