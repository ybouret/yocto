#ifndef YOCTO_TREE_FS_INCLUDED
#define YOCTO_TREE_FS_INCLUDED 1

#include "yocto/fs/vfs.hpp"
#include "yocto/core/list.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/container/sequence.hpp"

namespace yocto
{
    struct __TreeFS
    {
        static void SplitPath( sequence<string> &ids, const string &path);
    };
    
    template <typename T>
    class TreeFS
    {
    public:
        YOCTO_ARGUMENTS_DECL_T;
        class Node;
        class Nodes : public object, public core::list_of_cpp<Node>
        {
        public:
            inline explicit Nodes() throw() {}
            inline virtual ~Nodes() throw() {}
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Nodes);
        };
        
        class Node : public object
        {
        public:
            Node *next;
            Node *prev;
            const vfs::entry::attribute attr;
            const string                path;
            void                       *data;
            
            inline explicit Node(const string &nodePath, Node *root) :
            next(0), prev(0),
            attr( vfs::entry::is_dir ),
            path( vfs::to_directory(nodePath) )
            {
                data = new Nodes;
                try { setup(root); } catch(...) { kill(); throw; }
            }
            
            inline explicit Node(const char *nodePath, Node *root) :
            next(0), prev(0),
            attr( vfs::entry::is_dir ),
            path( vfs::to_directory(nodePath) )
            {
                data = new Nodes;
                try { setup(root); } catch(...) { kill(); throw; }
            }
            
            
            inline virtual ~Node() throw()
            {
                kill();
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Node);
            inline void setup(Node *root)
            {
                if(root)
                {
                    
                }
            }
            
            inline void kill() throw()
            {
                switch(attr)
                {
                    case vfs::entry::is_dir:
                        delete static_cast<Nodes *>(data);
                        data = 0;
                        break;
                        
                    case vfs::entry::is_reg:
                        delete static_cast<type *>(data);
                        data = 0;
                        break;
                        
                    default:
                        break;
                }
            }
        };
        
        
        inline explicit TreeFS() :
        root( new Node("/",NULL) )
        {
        
        }
        
        inline virtual ~TreeFS() throw()
        {
            
        }
        
    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(TreeFS);
        auto_ptr<Node> root;
    };
    
}

#endif

