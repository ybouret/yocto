#ifndef YOCTO_MULTI_SET_INCLUDED
#define YOCTO_MULTI_SET_INCLUDED 1

#include "yocto/associative/set.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/intr.hpp"
#include "yocto/exception.hpp"
#include "yocto/sequence/vector.hpp"

namespace yocto
{
    namespace hidden
    {
        extern const char multi_set_name[];
        extern const char multi_set_group[];
    }


    template <
    typename KEY,
    typename T,
    typename KEY_HASHER = key_hasher<KEY>,
    typename ALLOCATOR  = memory::global::allocator >
    class multi_set 
    {
    public:
        YOCTO_ASSOCIATIVE_KEY_T;


        //! a group will exist with at least one object
        class group : public array<T>, public counted
        {
        public:

            const_key group_key;
            inline const_key & key() const throw() { return group_key; }

            inline explicit group(param_key  k,
                                  param_type a) :
            group_key(k),
            content(8,as_capacity)
            {
                content.push_back(a);
            }

            inline virtual ~group() throw()
            {
            }

            inline virtual const char *name() const throw()     { return hidden::multi_set_group; }
            inline virtual size_t      size() const throw()     { return content.size();          }
            inline virtual size_t      capacity() const throw() { return content.capacity();      }
            inline virtual void        reserve(const size_t n)  { content.reserve(n);             }
            inline virtual void        release() throw()        { /* do nothing */                }
            inline virtual void        free()    throw()        { /* do nothing */                }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(group);
            vector<type,ALLOCATOR> content;
            virtual const_type *get_item() const throw() { return content.get_item(); }
            friend class multi_set;
        };


        //! types
        typedef intr_ptr<KEY,group>                    gPointer;
        typedef set<KEY,gPointer,KEY_HASHER,ALLOCATOR> gDataBase;
        typedef typename gDataBase::iterator           iterator;
        typedef typename gDataBase::const_iterator     const_iterator;

        virtual const char *name() const throw() { return hidden::multi_set_name; }


        inline explicit multi_set()
        {
        }

        inline virtual ~multi_set() throw()
        {
        }

        inline iterator       begin() throw()       { return gdb.begin(); }
        inline const_iterator begin() const throw() { return gdb.begin(); }
        inline iterator       end()   throw()       { return gdb.end();   }
        inline const_iterator end()   const throw() { return gdb.end();   }


        inline void insert(param_key key, param_type arg)
        {
            gPointer *ppG = gdb.search(key);
            if(ppG)
            {
                (*ppG)->content.push_back(arg);
            }
            else
            {
                gPointer pG( new group(key,arg) );
                if(!gdb.insert(pG))
                {
                    throw exception("%s: unexpected insert failure", name() );
                }
            }
        }

        inline const group *search(param_key key) const throw()
        {
            const gPointer *ppG = gdb.search(key);
            return (ppG) ? & **ppG : 0;
        }


    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(multi_set);
        gDataBase gdb;

        inline group *search(param_key key) throw()
        {
            gPointer *ppG = gdb.search(key);
            return (ppG) ? & **ppG : 0;
        }

    };
}

#endif

