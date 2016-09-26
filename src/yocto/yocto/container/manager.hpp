#ifndef YOCTO_CONTAINER_MANAGER_INCLUDED
#define YOCTO_CONTAINER_MANAGER_INCLUDED 1

#include "yocto/container/container.hpp"
#include "yocto/sequence/addr-list.hpp"

namespace yocto
{

    template <typename T>
    class container_manager_for
    {
    public:
        inline explicit container_manager_for() throw()
        {
        }

        inline virtual ~container_manager_for() throw()
        {

        }

        inline void enroll( T &host )
        {
            content.append( &host );
        }

        //dispatch methods
        inline void free_all() throw()
        {
            for(node_type *node=content.tail;node;node=node->prev)
            {
                node->addr->free();
            }
        }

        inline void release_all() throw()
        {
            for(node_type *node=content.tail;node;node=node->prev)
            {
                node->addr->release();
            }
        }

        inline void ensure_all(size_t n)
        {
            for(node_type *node=content.head;node;node=node->next)
            {
                node->addr->ensure(n);
            }
        }

        inline void reserve_all(size_t n)
        {
            for(node_type *node=content.head;node;node=node->next)
            {
                node->addr->reserve(n);
            }
        }


        inline void make_all(size_t n)
        {
            for(node_type *node=content.head;node;node=node->next)
            {
                node->addr->make(n);
            }
        }


    private:
        typedef addr_node<T> node_type;
        addr_list<T> content;

        YOCTO_DISABLE_COPY_AND_ASSIGN(container_manager_for);
    };

}

#endif
