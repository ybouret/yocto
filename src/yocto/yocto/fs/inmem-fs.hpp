#ifndef YOCTO_FS_INMEM_INCLUDED
#define YOCTO_FS_INMEM_INCLUDED 1

#include "yocto/fs/vfs.hpp"
#include "yocto/core/list.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/sequence/c-array.hpp"

namespace yocto
{
    //! in memory read-only fs
    class inmem_fs : public vfs
    {
    public:

        //typedef c_array<char> file_type;

        // node
        class node_type;
        typedef core::list_of_cpp<node_type> __nodes;
        class nodes : public object, public __nodes
        {
        public:
            explicit nodes() throw() : object(), __nodes() {}
            virtual ~nodes() throw() {}
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(nodes);
        };

        class node_type : public object
        {
        public:
            const entry::attribute attr;
            node_type             *next;
            node_type             *prev;
            const string           path;

            explicit node_type(const string &node_path);
            virtual ~node_type() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(node_type);
        };

        explicit inmem_fs();
        virtual ~inmem_fs() throw();

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(inmem_fs);
    };
}

#endif

