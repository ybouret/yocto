
#ifndef YOCTO_ASSOCIATIVE_HLIST_INCLUDED
#define YOCTO_ASSOCIATIVE_HLIST_INCLUDED 1

#include "yocto/core/list.hpp"
#include "yocto/associative/key-hasher.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/type/key.hpp"
#include "yocto/ptr/auto.hpp"

#include <iostream>

namespace yocto
{
    struct hlist_param
    {
        static const size_t load_factor = 4;
        static const size_t mini_klists = 8;
    };

    template <
    typename KEY,
    typename NODE,
    typename KEY_HASHER = key_hasher<KEY>
    >
    class hlist
    {
    public:
        YOCTO_ARGUMENTS_DECL_KEY;
        class kNode : public object
        {
        public:
            const_key    key;
            const size_t hkey;
            kNode       *next;
            kNode       *prev;
            NODE        *node;

            inline  kNode(param_key k, const size_t h, NODE *n)  :
            key(k), hkey(h), next(0), prev(0), node(n)
            {
                assert(n);
            }
            inline  virtual ~kNode() throw() {  }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(kNode);
        };
        typedef core::list_of_cpp<kNode> kList;
        typedef core::list_of_cpp<NODE>  list_type;
        
#define YOCTO_HLIST_CTOR() hash(), nodes(), klist(0), ksize(0), kmask(0),  nlist(0)
        inline explicit hlist() throw() :
        YOCTO_HLIST_CTOR()
        {
        }
        
        //! possible if NODE has a copy constructor
        inline hlist(const hlist &other) :
        YOCTO_HLIST_CTOR()
        {
            const size_t n = other.size();
            if(n>0)
            {
                try
                {
                    allocate_lists(n/hlist_param::load_factor);
                    assert(ksize>0);
                    for(size_t i=0;i<other.ksize;++i)
                    {
                        const kList &l = other.klist[i];
                        for(const kNode *kn=l.head;kn;kn=kn->next)
                        {
                            const_key   &k   = kn->key;
                            const size_t h   = kn->hkey;
                            NODE        *node = new NODE( *(kn->node) );
                            auto_ptr<NODE> guard(node);
                            klist[h&kmask].push_back( new kNode(k,h,node) );
                            nodes.push_back( guard.yield() );
                        }
                    }
                    assert( this->size() == other.size() );
                }
                catch(...)
                {
                    release();
                    throw;
                }
            }
        }
        

        inline bool insert(param_key key,NODE *node)
        {
            auto_ptr<NODE> guard(node);
            check_load_factor();
            // look up klist
            const size_t   h =  hash(key);
            kList         *l = &klist[h&kmask];

            // check key
            for(const kNode *kn=l->head;kn;kn=kn->next)
            {
                if(kn->key == key) return false; //!< NODE is erased
            }

            // ok
            l->push_front( new kNode(key,h,node) );
            nodes.push_back( guard.yield() );
            return true;
        }

        inline size_t size() const throw() { return nodes.size; }

        inline void free() throw()
        {
            nodes.clear(); assert(0==nodes.size);
            for(size_t i=0;i<ksize;++i) { klist[i].clear(); assert(0==klist[i].size); }
        }

        inline void release() throw()
        {
            free();
            memory::kind<memory::global>::release_as(klist,nlist);
            kmask = 0;
        }

        virtual ~hlist() throw()
        {
            release();
        }

        inline NODE *search(param_key key) const throw()
        {
            if(ksize>0)
            {
                const size_t h = hash(key);
                const kList &l = klist[h&kmask];
                for(const kNode *kn = l.head;kn;kn=kn->next)
                {
                    if(kn->key==key)
                    {
                        return kn->node;
                    }
                }
                return NULL;
            }
            else
            {
                return NULL;
            }
        }

        bool remove( param_key key ) throw()
        {
            if(ksize)
            {
                const size_t h = hash(key);
                kList       &l = klist[h&kmask];
                for(kNode *kn = l.head;kn;kn=kn->next)
                {
                    if( kn->key == key )
                    {
                        delete nodes.unlink(kn->node);
                        delete l.unlink(kn);
                        return true;
                    }
                }
                return false;
            }
            else
            {
                return NULL;
            }
        }

    private:
        mutable KEY_HASHER hash;
        list_type   nodes;
        kList      *klist; //!< slots for hashing
        size_t      ksize; //!< a power of 2
        size_t      kmask; //!< ksize-1
        size_t      nlist; //!< in memory, nlist>=ksize

        YOCTO_DISABLE_ASSIGN(hlist);

        inline void allocate_lists(const size_t n)
        {
            const size_t ksize_req = next_power_of_two(n);
            if(ksize_req!=ksize)
            {
                // compute requested data
                size_t       nlist_req = ksize_req;
                kList *      klist_req = memory::kind<memory::global>::acquire_as<kList>(nlist_req);
                const size_t kmask_req = ksize_req - 1;

                // transfer kNodes
                for(size_t i=0;i<ksize;++i)
                {
                    kList &src = klist[i];
                    while(src.size>0)
                    {
                        kNode *knode = src.pop_front(); assert(i==(knode->hkey&kmask));
                        klist_req[knode->hkey&kmask_req].push_back(knode);
                    }
                }

                // update
                memory::kind<memory::global>::release_as(klist,nlist);
                klist = klist_req;
                ksize = ksize_req;
                nlist = nlist_req;
                kmask = kmask_req;

                size_t count = 0;
                for(size_t i=0;i<ksize;++i)
                {
                    count += klist[i].size;
                }
                assert(nodes.size==count);

            }

        }

        inline void check_load_factor()
        {
            const size_t new_size  = nodes.size+1;
            const size_t max_capa  = ksize*hlist_param::load_factor;
            if(new_size>=max_capa)
            {
                allocate_lists(max_of(max_capa,hlist_param::mini_klists));
            }
        }


    };
}

#endif

