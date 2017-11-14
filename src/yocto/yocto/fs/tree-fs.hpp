#ifndef YOCTO_FS_TREEFS_INCLUDED
#define YOCTO_FS_TREEFS_INCLUDED 1

#include "yocto/fs/vfs.hpp"
#include "yocto/container/vslot.hpp"
#include "yocto/core/list.hpp"

namespace yocto
{

    //! abstract class to search/create paths as a tree
    class TreeFS
    {
    public:
        class Node;
        typedef core::list_of_cpp<Node> Nodes;
        class Node
        {
        public:
            const vfs::entry::attribute attr; //!< is_dir or is_reg
            Node                       *next;
            Node                       *prev;
            const string                path;
#if 0
            Nodes                       entries;
            vslot                       content;
#else
            void *data;
#endif

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Node);
        };
        //! create the root node
        explicit TreeFS();
        virtual ~TreeFS() throw();



    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(TreeFS);
    };
}
#endif

