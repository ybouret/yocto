#ifndef YOCTO_FS_INMEM_INCLUDED
#define YOCTO_FS_INMEM_INCLUDED 1

#include "yocto/fs/vfs.hpp"

namespace yocto
{
    class inmem_fs : public vfs
    {
    public:
        explicit inmem_fs();
        virtual ~inmem_fs() throw();

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(inmem_fs);
    };
}

#endif

