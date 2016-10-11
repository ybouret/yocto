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
        typedef container_manager_of<T> self_type;

        inline explicit container_manager_of() throw() : container_manager()
        {
        }

        inline virtual ~container_manager_of() throw()
        {

        }

        //! enroll a host in groups corresponding to mask
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


        //! free all groups
        inline void free_all() throw()
        {
            __all_no_throw(__free_all);
        }

        //! free all groups matching the mask
        inline void free_all( group_t group_mask ) throw()
        {
            __all_no_throw(__free_all,group_mask);
        }

        //! release all groups
        inline void release_all() throw()
        {
            __all_no_throw(__release_all);
        }

        //! release all groups corresponding to mask
        inline void release_all( group_t group_mask ) throw()
        {
            __all_no_throw(__release_all,group_mask);
        }

        inline void ensure_all(size_t n)
        {
            __all(__ensure_all,n);
        }

        inline void ensure_all( group_t group_mask, const size_t n )
        {
            __all(__ensure_all,group_mask,n);
        }


        inline void reserve_all(size_t n)
        {
            __all(__reserve_all,n);
        }

        inline void reserve_all( group_t group_mask, const size_t n )
        {
            __all(__reserve_all,group_mask,n);
        }
        

        inline void make_all(size_t n)
        {
            __all(__make_all,n);
        }

        inline void make_all( group_t group_mask, const size_t n )
        {
            __all(__make_all,group_mask,n);
        }
        



    private:
        typedef addr_node<T> node_type;
        typedef addr_list<T> group;

        group                groups[num_groups];
        
        YOCTO_DISABLE_COPY_AND_ASSIGN(container_manager_of);
        template <typename PROC>
        inline void __all_no_throw(const PROC proc) throw()
        {
            for(size_t i=0;i<num_groups;++i)
            {
                proc(groups[i]);
            }
        }

        template <typename PROC>
        inline void __all_no_throw(const PROC proc,group_t group_mask) throw()
        {
            group_mask = check_group(group_mask);
            for(size_t i=0;i<num_groups;++i)
            {
                if(belongs_to(i,group_mask))
                {
                    proc(groups[i]);
                }
            }
        }

        template <typename PROC>
        inline void __all(const PROC proc, const size_t n)
        {
            try
            {
                for(size_t i=0;i<num_groups;++i)
                {
                    proc(groups[i],n);
                }
            }
            catch(...)
            {
                release_all();
                throw;
            }
        }

        template <typename PROC>
        inline void __all(const PROC proc,group_t group_mask,const size_t n)
        {
            try
            {
                group_mask = check_group(group_mask);
                for(size_t i=0;i<num_groups;++i)
                {
                    if(belongs_to(i,group_mask))
                    {
                        proc(groups[i],n);
                    }
                }
            }
            catch(...)
            {
                release_all();
                throw;
            }
        }


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
