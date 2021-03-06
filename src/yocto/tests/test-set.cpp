#include "yocto/utest/run.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/hashing/elf.hpp"
#include "yocto/hashing/sfh.hpp"
#include "yocto/hashing/sha1.hpp"
#include "yocto/memory/pooled.hpp"
#include "yocto/sequence/list.hpp"
#include "yocto/comparator.hpp"

#include "support.hpp"
#include <typeinfo>


using namespace yocto;


namespace
{
	template <typename KEY>
	class dummy
	{
	public:
		YOCTO_ARGUMENTS_DECL_KEY;
		
		KEY id;
		
		dummy( param_key k ) : id(k)
		{
		}
		
		dummy( const dummy &other ) : id( other.id ) {}
		
		~dummy() throw() {}
		
		const_key & key() const throw() { return id; }

        inline friend std::ostream & operator<<( std::ostream &os, const dummy &d )
        {
            os << d.id;
            return os;
        }

        static inline
        int cmp_data( const dummy &lhs, const dummy &rhs)
        {
            return __compare<KEY>(lhs.id,rhs.id);
        }

        static inline
        int cmp_keys( const dummy &lhs, const dummy &rhs)
        {
            return __compare<KEY>(rhs.id,lhs.id);
        }

	private:
		YOCTO_DISABLE_ASSIGN(dummy);
		
	};
	
	template <typename KEY, typename KEY_HASHER, typename ALLOCATOR>
	static inline void test_set()
	{
		typedef set<KEY,dummy<KEY>,KEY_HASHER,ALLOCATOR> set_t;
		typedef typename set_t::mutable_key              key_t;

        list<KEY> key_seq;

        for(size_t iter=0;iter<1;++iter)
        {
            vector<KEY> keys;
            set_t       S;
            std::cerr << "set<" << typeid(KEY).name() << ",dummy>" << std::endl;
            std::cerr << "-- insert" << std::endl;
            for( size_t i=0; i < 4096; ++i )
            {
                KEY        k = gen<key_t>::get();
                dummy<KEY> d( k );
                if( S.insert( d ) )
                {
                    if( alea.get<float>() > 0.6f ) keys.push_back( k );
                }
                else {
                    std::cerr << "Multiple Key '" << k << "'" << std::endl;
                }
            }
            std::cerr << "-- size=" << S.size() << "/" << S.capacity() << std::endl;
            const size_t sig1 = S.signature();
            set_t S2(S);
            const size_t sig2 = S.signature();
            std::cerr << "signatures: " << sig1 << "/" << sig2 << std::endl;
            std::cerr << "---- removing " << keys.size() << " keys" << std::endl;
            for( size_t i=1; i <= keys.size(); ++i )
            {
                if( !S2.remove( keys[i] ) ) std::cerr << "can't remove " << keys[i] << std::endl;
            }
            
            for( typename set_t::iterator i = S2.begin(); i != S2.end(); ++i )
            {
                const KEY &k = i->data.key();
                if( ! S.remove( k ) ) std::cerr << "can't remove " << k << std::endl;
            }

            key_seq.free();
            S.collect_keys(key_seq);
        }

        std::cerr << std::endl;
	}


}

#include "yocto/associative/key-dumper.hpp"
#include "yocto/code/alea.hpp"

YOCTO_UNIT_TEST_IMPL(set)
{
	
	test_set<int,key_hasher<int,hashing::sfh>,memory::global::allocator>();
	test_set<string,key_hasher<string,hashing::elf>,memory::global::allocator>();
	test_set<const string,key_hasher<string,hashing::sha1>,memory::pooled::allocator>();
    test_set<short,key_dumper<short>,memory::global::allocator>();

    {
        set<string, dummy<string> > ds;
        for(size_t i=0;i<10;++i)
        {
            const string k = gen<string>::get();
            const string a = gen<string>::get();
            const dummy<string> d(a);
            (void) ds.insert(d);
        }
        std::cerr << ds << std::endl;

        ds.sort_data_by(dummy<string>::cmp_data);
        std::cerr << ds << std::endl;

        ds.sort_keys_by(dummy<string>::cmp_keys);
        std::cerr << ds << std::endl;
    }



}
YOCTO_UNIT_TEST_DONE()

