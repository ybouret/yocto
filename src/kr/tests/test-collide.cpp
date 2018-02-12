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

using namespace yocto;


YOCTO_UNIT_TEST_IMPL(collide)
{
    hashing::adler32 h_adler32;
    hashing::bjh32   h_bjh32;
    hashing::crc16   h_crc16;
    hashing::crc32   h_crc32;
    hashing::elf     h_elf;
    hashing::fnv     h_fnv;
    hashing::sfh     h_sfh;
    hashing::md2     h_md2;
    hashing::md4     h_md4;
    hashing::md5     h_md5;
    hashing::sha1    h_sha1;
    hashing::sha224  h_sha224;
    hashing::sha256  h_sha256;
    hashing::sha384  h_sha384;
    hashing::sha512  h_sha512;
    hashing::rmd160  h_rmd160;
    hashing::rmd128  h_rmd128;
}
YOCTO_UNIT_TEST_DONE()

