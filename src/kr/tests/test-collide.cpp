#include "yocto/utest/run.hpp"
#include "yocto/hashing/sha1.hpp"
#include "yocto/hashing/md2.hpp"
#include "yocto/hashing/md5.hpp"
#include "yocto/hashing/md4.hpp"
#include "yocto/hashing/sha256.hpp"
#include "yocto/hashing/sha512.hpp"
#include "yocto/hashing/rmd128.hpp"
#include "yocto/hashing/rmd160.hpp"
#include "yocto/hashing/adler32.hpp"
#include "yocto/hashing/bjh32.hpp"
#include "yocto/hashing/crc16.hpp"
#include "yocto/hashing/crc32.hpp"
#include "yocto/hashing/elf.hpp"
#include "yocto/hashing/fnv.hpp"
#include "yocto/hashing/hash64.hpp"
#include "yocto/hashing/sfh.hpp"

#include "yocto/hashing/factory.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/sys/wtime.hpp"
#include "yocto/hashing/digest.hpp"
#include "yocto/sort/heap.hpp"

using namespace yocto;

#define _HASH(NAME) do { \
hashing::function *h = new hashing::NAME();\
F.append( h );  \
std::cerr << "|_" << h->name() << std::endl;\
} while(false)

#if 0
static inline void test_words( hashing::function &F, const array<string> &words )
{
    std::cerr << F.name() << std::endl;
    wtime chrono;
    chrono.start();
    const uint64_t mark = wtime::ticks();
    for(size_t i=words.size();i>0;--i)
    {
        (void)F.key<size_t>(words[i]);
    }
    const double ell = chrono(wtime::ticks() - mark);
    std::cerr << "\t --> ellapsed  = " << ell*1000 << "ms" << std::endl;

    map<uint32_t,bool> keys(words.size(),as_capacity);
    size_t count = 0;
    for(size_t i=words.size();i>0;--i)
    {
        uint32_t k32 = F.key<uint32_t>(words[i]);
        if(!keys.insert(k32,true))
        {
            ++count;
        }
    }
    std::cerr << "\t --> collide32 = " << count << std::endl;

    std::cerr << std::endl;
}
#endif

#include "yocto/code/utils.hpp"

namespace
{
    class h_entry : public counted_object
    {
    public:
        typedef arc_ptr<h_entry> ptr;

        const string word;
        const digest full;
        const string hash;

        explicit h_entry(hashing::function &h, const string &w) :
        word(w),
        full( digest::checksum(h,word) ),
        hash( d2s(full) )
        {

        }


    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(h_entry);
        static string d2s( const digest &d )
        {
            const size_t n = d.length();
            string s(n<<1,as_capacity);
            for(size_t i=0;i<n;++i)
            {
                s << hexa_text[d[i]];
            }
            return s;
        }
    };

}


static inline int compare_hptr( const h_entry::ptr &lhs, const h_entry::ptr &rhs)
{
    return string::compare(lhs->hash,rhs->hash);
}

YOCTO_UNIT_TEST_IMPL(collide)
{
    hashing::factory F(32);
    _HASH(adler32);
    _HASH(bjh32);
    _HASH(crc16);
    _HASH(crc32);
    _HASH(elf);
    _HASH(fnv);
    _HASH(sfh);
    _HASH(md2);
    _HASH(md4);
    _HASH(md5);
    _HASH(sha1);
    _HASH(sha224);
    _HASH(sha256);
    _HASH(sha384);
    _HASH(sha512);
    _HASH(rmd160);
    _HASH(rmd128);



    if(argc>1)
    {
        hashing::function &h = F[ argv[1] ];
        std::cerr << "using " << h.name() << std::endl;
        if(argc>2)
        {
            vector<h_entry::ptr> entries(500000,as_capacity);
            {
                ios::icstream fp(argv[2]);
                string line;
                while(fp.gets(line))
                {
                    const h_entry::ptr p( new h_entry(h,line) );
                    entries.push_back(p);
                    //std::cerr << p->full << " " << p->hash << " '" << p->word << "'" << std::endl;
                }
            }
            std::cerr << "generated " << entries.size() << " entries" << std::endl;
            std::cerr << "sorting..." << std::endl;
            hsort(entries,compare_hptr);
            for(size_t i=1;i<=entries.size();++i)
            {
                const h_entry::ptr &p = entries[i];
                std::cerr << p->hash << " '" << p->word << "'" << std::endl;
            }
        }
    }




}
YOCTO_UNIT_TEST_DONE()

