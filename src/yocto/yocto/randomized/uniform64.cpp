#include "yocto/randomized/uniform64.hpp"

namespace yocto {

    namespace Randomized {

#define YOCTO_RANDOM_HASH64_IMPL(CLASS_NAME, HASH_NAME) \
CLASS_NAME:: CLASS_NAME() throw() : hprng<HASH_NAME>() {} \
CLASS_NAME::~CLASS_NAME() throw() {}

        YOCTO_RANDOM_HASH64_IMPL(Uniform64BJ,hashing::hash64::BJ)
        YOCTO_RANDOM_HASH64_IMPL(Uniform64NR,hashing::hash64::NR)
    }

}
