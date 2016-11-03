#ifndef  YOCTO_PARALLEL_SERVER_SPLIT_INCLUDED
#define  YOCTO_PARALLEL_SERVER_SPLIT_INCLUDED 1

#include "yocto/parallel/splitter.hpp"
#include "yocto/threading/scheme/server.hpp"

namespace yocto
{
    namespace parallel
    {

        template <
        typename USER_PATCH,
        typename PATCH_TYPE
        >
        void create_patches(vector<USER_PATCH> &patches,
                            const PATCH_TYPE   &zone,
                            threading::server  *server)
        {
            const size_t cpus = (0!=server) ? server->cpu.num_threads() : 1;
            build_patches<USER_PATCH,PATCH_TYPE>(patches,cpus,zone);
        }

    }
}

#endif
