
#include "yocto/ink/types.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/hashing/sfh.hpp"
#include "yocto/hashing/sha1.hpp"
#include "yocto/hashing/sha1.hpp"
#include "yocto/hashing/pjw.hpp"
#include "yocto/hashing/fnv.hpp"
#include "yocto/sys/timings.hpp"
#include "yocto/ordered/sorted-vector.hpp"
#include "yocto/associative/map.hpp"

using namespace yocto;
using namespace Ink;

#define SCREEN_SIZE 4096
#define SCREEN_NVTX (SCREEN_SIZE*SCREEN_SIZE)

static inline
void hashRun( hashing::function &h, const coord *v)
{
    timings tmx;
    YOCTO_TIMINGS(tmx,1,
    h.set();
    for(size_t i=0;i<SCREEN_NVTX;++i)
    {
        h.run( &v[i], sizeof(coord) );
    });

    std::cerr << h.name() << "\t: speed : " << tmx.speed << std::endl;

}


static inline
void hashCollide(hashing::function &h, const coord *v)
{
    //sorted_vector<size_t> keys(SCREEN_NVTX,as_capacity);
    map<size_t,size_t> keys(SCREEN_NVTX,as_capacity);
    size_t count = 0;
    for(size_t i=0;i<SCREEN_NVTX;++i)
    {
        const size_t k = h.key<size_t>(&v[i],sizeof(coord));
        if(!keys.insert(k,k)) ++count;
    }
    std::cerr << h.name() << "#collisions : " << count << std::endl;
}

YOCTO_UNIT_TEST_IMPL(mask)
{
    std::cerr << "Generating " << SCREEN_NVTX << " coords" << std::endl;
    vector<coord> vtx(SCREEN_NVTX);
    {
        size_t indx = 0;
        for(unit_t x=0;x<SCREEN_SIZE;++x)
        {
            for(unit_t y=0;y<SCREEN_SIZE;++y)
            {
                vtx[++indx] = coord(x,y);
            }
        }
    }

    const coord *v = &vtx[1];
    std::cerr << "Timing hash functions" << std::endl;
    hashing::sfh  SFH;  hashRun(SFH,v);
    hashing::sha1 SHA1; hashRun(SHA1,v);
    hashing::fnv  FNV;  hashRun(FNV,v);
    hashing::pjw  PJW;  hashRun(PJW,v);
    std::cerr << "Collisions" << std::endl;
    hashCollide(SFH,v);
    hashCollide(SHA1,v);
    hashCollide(FNV,v);
    hashCollide(PJW,v);

}
YOCTO_UNIT_TEST_DONE()


