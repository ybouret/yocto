#include "yocto/utest/run.hpp"
#include "yocto/ordered/sorted-vector.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/alea.hpp"
#include "support.hpp"
#include "yocto/code/utils.hpp"

#include <typeinfo>

using namespace yocto;
template <typename T>
static inline void check_indices( const sorted_vector<T> &sv )
{
    std::cerr << "check indices..." << std::endl;
    for(size_t i=sv.size();i>0;--i)
    {
        const T &key = sv[i];
        if( sv.index_of(key) != i )
        {
            throw exception("mismatch key index#%u", unsigned(i));
        }
    }
}

YOCTO_UNIT_TEST_IMPL(sorted_vec)
{

    const size_t nmax = 10000;
	{
		sorted_vector<size_t> sv;
		size_t count = 0;
		for( size_t i=0; i < nmax; ++i)
		{
			if( sv.insert( alea.lt(nmax) ) )
			{
				++count;
			}
		}
		std::cerr << "inserted " << count << "/" << nmax << std::endl;
		assert( sv.size() == count );
        check_indices(sv);

		const size_t step = max_of<size_t>(1,count/10);
		for( size_t i=1; i <= count; i += step )
		{
			std::cerr << sv[i] << std::endl;
		}
		sorted_vector<size_t> sv2( sv );
		sv2.free();
	}
	
	{
		sorted_vector<string> sv;
		size_t count = 0;
		for( size_t i=0; i < nmax; ++i)
		{
			const string tmp = gen<string>::get();
			if( sv.insert( tmp ) )
			{
				++count;
			}
		}
		std::cerr << "inserted " << count << "/" << nmax << std::endl;
		assert( sv.size() == count );
        check_indices(sv);
		const size_t step = max_of<size_t>(1,count/10);
		for( size_t i=1; i <= count; i += step )
		{
			std::cerr << sv[i] << std::endl;
		}
		sorted_vector<string> sv2( sv );
		sv2.free();
	}
}
YOCTO_UNIT_TEST_DONE()
