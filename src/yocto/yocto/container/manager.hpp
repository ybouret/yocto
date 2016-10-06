#ifndef YOCTO_CONTAINER_MANAGER_INCLUDED
#define YOCTO_CONTAINER_MANAGER_INCLUDED 1

#include "yocto/container/container.hpp"
#include "yocto/sequence/addr-list.hpp"

namespace yocto
{

    class container_manager
    {
    public:
        typedef uint16_t     group_t;
        static const size_t  num_groups    = sizeof(group_t)*8;
        static const group_t default_group = 0x01;
        virtual ~container_manager() throw();
    protected:
        explicit container_manager() throw();

        //! i = #group, group_id>0
        static bool    belongs_to(const size_t i, const group_t group_id ) throw();
        static group_t check_group(group_t group_id) throw();
    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(container_manager);
    };
    
    template <typename T>
    class container_manager_of : public container_manager
    {
    public:
        inline explicit container_manager_of() throw() : container_manager()
        {
        }

        inline virtual ~container_manager_of() throw()
        {

        }

        // enroll a host in
        inline void enroll( T &host, group_t group_mask )
        {
            group_mask = check_group(group_mask);
            for(size_t i=0;i<num_groups;++i)
            {
                if(belongs_to(i,group_mask))
                {
                    groups[i].append( &host );
                }
            }
        }

        // free all groups
        inline void free_all() throw()
        {
            for(size_t i=0;i<num_groups;++i)
            {
                __free_all(groups[i]);
            }
        }

        inline void free_all( group_t group_mask ) throw()
        {
            group_mask = check_group(group_mask);
            for(size_t i=0;i<num_groups;++i)
            {
                if(belongs_to(i,group_mask))
                {
                    __free_all(groups[i]);
                }
            }

        }

        inline void release_all() throw()
        {
            for(size_t i=0;i<num_groups;++i)
            {
                __release_all(groups[i]);
            }
        }

        inline void release_all( group_t group_mask ) throw()
        {
            group_mask = check_group(group_mask);
            for(size_t i=0;i<num_groups;++i)
            {
                if(belongs_to(i,group_mask))
                {
                    __release_all(groups[i]);
                }
            }

        }

        inline void ensure_all(size_t n)
        {
            try
            {
                for(size_t i=0;i<num_groups;++i)
                {
                    __ensure_all(groups[i],n);
                }
            }
            catch(...)
            {
                release_all();
                throw;
            }
        }

        inline void ensure_all( group_t group_mask, const size_t n )
        {
            try
            {
                group_mask = check_group(group_mask);
                for(size_t i=0;i<num_groups;++i)
                {
                    if(belongs_to(i,group_mask))
                    {
                        __ensure_all(groups[i],n);
                    }
                }
            }
            catch(...)
            {
                release_all();
                throw;
            }

        }


        inline void reserve_all(size_t n)
        {
            try
            {
                for(size_t i=0;i<num_groups;++i)
                {
                    __reserve_all(groups[i],n);
                }
            }
            catch(...)
            {
                release_all();
                throw;
            }
        }

        inline void reserve_all( group_t group_mask, const size_t n )
        {
            try
            {
                group_mask = check_group(group_mask);
                for(size_t i=0;i<num_groups;++i)
                {
                    if(belongs_to(i,group_mask))
                    {
                       __reserve_all(groups[i],n);
                    }
                }
            }
            catch(...)
            {
                release_all();
                throw;
            }

        }
        

        inline void make_all(size_t n)
        {
            try
            {
                for(size_t i=0;i<num_groups;++i)
                {
                    __make_all(groups[i],n);
                }
            }
            catch(...)
            {
                release_all();
                throw;
            }
        }

        inline void make_all( group_t group_mask, const size_t n )
        {
            try
            {
                group_mask = check_group(group_mask);
                for(size_t i=0;i<num_groups;++i)
                {
                    if(belongs_to(i,group_mask))
                    {
                        __make_all(groups[i],n);
                    }
                }
            }
            catch(...)
            {
                release_all();
                throw;
            }

        }
        



    private:
        typedef addr_node<T> node_type;
        typedef addr_list<T> group;
        
        group                groups[num_groups];
        
        YOCTO_DISABLE_COPY_AND_ASSIGN(container_manager_of);
        static inline void __free_all( group &g ) throw()
        {
            for(node_type *node=g.tail;node;node=node->prev)
            {
                node->addr->free();
            }
        }

        static inline void __release_all( group &g ) throw()
        {
            for(node_type *node=g.tail;node;node=node->prev)
            {
                node->addr->release();
            }
        }

        static inline void __ensure_all( group &g, const size_t n )
        {
            for(node_type *node=g.head;node;node=node->next)
            {
                node->addr->ensure(n);
            }
        }

        static inline void __reserve_all( group &g, const size_t n )
        {
            for(node_type *node=g.head;node;node=node->next)
            {
                node->addr->reserve(n);
            }
        }

        static inline void __make_all( group &g, const size_t n )
        {
            for(node_type *node=g.head;node;node=node->next)
            {
                node->addr->make(n);
            }
        }

    };

}

#endif
