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
            inline virtual void        reserve( size_t n)  { content.reserve(n);             }
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

        //! name for sub-class
        virtual const char *name() const throw() { return hidden::multi_set_name; }


        inline explicit multi_set() throw(): gdb()
        {
        }

        inline virtual ~multi_set() throw()
        {
        }

        inline iterator       begin() throw()       { return gdb.begin(); }
        inline const_iterator begin() const throw() { return gdb.begin(); }
        inline iterator       end()   throw()       { return gdb.end();   }
        inline const_iterator end()   const throw() { return gdb.end();   }

        inline size_t groups()   const throw() { return gdb.size(); }
        inline size_t stored()   const throw()
        {
            size_t count = 0;
            for(const_iterator i=gdb.begin();i!=gdb.end();++i)
            {
                count += (*i)->size();
            }
            return count;
        }

        //! insert a pair key/value
        inline void insert(param_key key, param_type value)
        {
            gPointer *ppG = gdb.search(key);
            if(ppG)
            {
                (*ppG)->content.push_back(value);
            }
            else
            {
                gPointer pG( new group(key,value) );
                if(!gdb.insert(pG))
                {
                    throw exception("%s: unexpected insert failure", name() );
                }
            }
        }


        //! get a group
        inline const group *search(param_key key) const throw()
        {
            const gPointer *ppG = gdb.search(key);
            return (ppG) ? & **ppG : 0;
        }

        //! remove bad key/value
        template <typename FUNC>
        inline void __remove_if( param_key key, FUNC &is_bad_data ) throw()
        {
            gPointer *ppG = gdb.search(key);
            if(!ppG) return;
            group    &g   = **ppG;
            g.content.__remove_if(is_bad_data);
            if(g.content.size()<=0)
            {
                (void) gdb.remove(key);
                assert(NULL==gdb.search(key));
            }
        }

        template <typename FUNC>
        inline void __remove_if( FUNC &is_bad_data )
        {
            vector<const_key,ALLOCATOR> keys(gdb.size(),as_capacity);
            for(iterator i=gdb.begin();i!=gdb.end();++i)
            {
                keys.push_back( (*i)->group_key );
            }
            for(size_t i=1;i<=keys.size();++i)
            {
                __remove_if(keys[i],is_bad_data);
            }
        }

        inline friend
        std::ostream & operator <<( std::ostream &os, const multi_set &ms )
        {
            os << '{';
            for( const_iterator i=ms.begin();i!=ms.end();++i)
            {
                const group    &g = **i;
                const array<T> &a = g;

                os << ' ' << '(' << g.key()  << ':' << a << ')';
            }
            os << ' ' << '}';
            return os;
        }

        template <typename FUNC>
        void sort( FUNC &func )
        {
            gdb.template sort_keys_by<FUNC>(func);
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

