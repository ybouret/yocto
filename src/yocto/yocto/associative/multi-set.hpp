#ifndef YOCTO_MULTI_SET_INCLUDED
#define YOCTO_MULTI_SET_INCLUDED 1

#include "yocto/associative/set.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/intr.hpp"

namespace yocto
{
    namespace hidden
    {
        extern const char multi_set_name[];
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

        class group : public counted_object
        {
        public:

            const_key group_key;
            inline const_key & key() const throw() { return group_key; }

            inline explicit group(param_key k) : group_key(k)
            {
            }

            inline virtual ~group() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(group);
        };

        typedef intr_ptr<KEY,group>                    gPointer;
        typedef set<KEY,gPointer,KEY_HASHER,ALLOCATOR> gDataBase;

        inline explicit multi_set()
        {
        }

        inline virtual ~multi_set() throw()
        {
        }


        inline group *search(param_key key) throw()
        {
            gPointer *ppG = gdb.search(key);
            return (ppG) ? & **ppG : 0;
        }

        inline const group *search(param_key key) const throw()
        {
            const gPointer *ppG = gdb.search(key);
            return (ppG) ? & **ppG : 0;
        }


#if 0
        //! get or create a new group on-the-fly
        group & operator[](param_key key)
        {
            gPointer *ppG = gdb.search(key);
            if(!ppG)
            {
                gPointer pG( new group(key) );
                if(!gdb.insert(pG)) throw exception("%s: unexpected new group failure", this->name().c_str() );
                return *pG;
            }
            else
            {
                return **ppG;
            }
        }
#endif

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(multi_set);
        gDataBase gdb;
    };
}

#endif

