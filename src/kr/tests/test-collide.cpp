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

using namespace yocto;

#define _HASH(NAME) F.append( new hashing::NAME() )

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
    }

#if 0
    if(argc>1)
    {
        vector<string> words(500000,as_capacity);
        {
            ios::icstream fp(argv[1]);
            string line;
            while(fp.gets(line))
            {
                words.push_back(line);
            }
            std::cerr << "testing " << words.size() << " words" << std::endl;
            for( hashing::factory::iterator i=F.begin(); i!=F.end();++i)
            {
                test_words( **i, words );
            }

        }
    }
#endif

}
YOCTO_UNIT_TEST_DONE()

