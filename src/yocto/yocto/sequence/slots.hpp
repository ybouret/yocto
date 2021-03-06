#ifndef YOCTO_SLOTS_INCLUDED
#define YOCTO_SLOTS_INCLUDED 1

#include "yocto/container/cslot.hpp"
#include "yocto/type/args.hpp"
#include "yocto/code/bswap.hpp"
#include <iostream>

namespace yocto
{
    template <typename T>
    class slots_of
    {
    public:
        YOCTO_ARGUMENTS_DECL_T;
        const size_t size;       //!< variable, 0..capacity
        const size_t capacity;   //!< max #slots

        inline explicit slots_of(size_t n=0) :
        size(0),
        capacity(n),
        cmem(n*sizeof(T))
        {
            assert(cmem.size>=n*sizeof(T));
        }

        inline type & operator[](const size_t indx) throw()
        {
            assert(indx<size);
            return *(static_cast<type *>(cmem.data)+indx);
        }

        inline const_type & operator[](const size_t indx) const throw()
        {
            assert(indx<size);
            return  *(static_cast<const_type *>(cmem.data)+indx);
        }

        //! append by copy
        inline type & push_back( param_type args )
        {
            assert(size<capacity);
            size_t &n = (size_t&)size;
            type   *p = new (static_cast<mutable_type *>(cmem.data)+n) mutable_type(args);
            ++n;
            return *p;
        }

        //! append by empty constructor
        inline type &push_back()
        {
            assert(size<capacity);
            size_t &n = (size_t&)size;
            type   *p = new (static_cast<mutable_type *>(cmem.data)+n) mutable_type();
            ++n;
            return *p;
        }

        //! append by one arg constructor
        template <typename U>
        inline type & append( typename type_traits<U>::parameter_type arg1 )
        {
            assert(size<capacity);
            size_t &n = (size_t&)size;
            type   *p = new (static_cast<mutable_type *>(cmem.data)+n) mutable_type(arg1);
            ++n;
            return *p;
        }

        //! append by two args constructor
        template <typename U,typename V>
        inline type & append(typename type_traits<U>::parameter_type arg1,
                             typename type_traits<V>::parameter_type arg2
                             )
        {
            assert(size<capacity);
            size_t &n = (size_t&)size;
            type   *p = new (static_cast<mutable_type *>(cmem.data)+n) mutable_type(arg1,arg2);
            ++n;
            return *p;
        }

        //! append by three args constructor
        template <typename U,typename V,typename W>
        inline type &append(typename type_traits<U>::parameter_type arg1,
                            typename type_traits<V>::parameter_type arg2,
                            typename type_traits<W>::parameter_type arg3
                            )
        {
            assert(size<capacity);
            size_t &n = (size_t&)size;
            type   *p = new (static_cast<mutable_type *>(cmem.data)+n) mutable_type(arg1,arg2,arg3);
            ++n;
            return *p;
        }

        //! append by four args constructor
        template <typename U,typename V,typename W, typename X>
        inline type & append(typename type_traits<U>::parameter_type arg1,
                             typename type_traits<V>::parameter_type arg2,
                             typename type_traits<W>::parameter_type arg3,
                             typename type_traits<X>::parameter_type arg4
                             )
        {
            assert(size<capacity);
            size_t &n = (size_t&)size;
            type   *p = new (static_cast<mutable_type *>(cmem.data)+n) mutable_type(arg1,arg2,arg3,arg4);
            ++n;
            return *p;
        }



        //! dectructor
        inline virtual ~slots_of() throw()
        {
            __free();
        }

        //! helper
        inline bool is_empty() const throw()
        {
            return (size<=0);
        }


        //! resize if EMPTY
        inline void resize_empty_to(size_t n)
        {
            assert(this->is_empty());
            if(n!=capacity)
            {
                slots_of<T> tmp(n);
                __swap_with(tmp);
            }
        }

        //! helper: sequential build from precomputed other slots
        template <typename S>
        inline void build_from( const slots_of<S> &src )
        {
            assert(this->is_empty());
            if(size<src.size) resize_empty_to(src.size);
            try
            {
                for(size_t i=0;i<src.size;++i)
                {
                    append<S>(src[i]);
                }
            }
            catch(...)
            {
                __free();
                throw;
            }
        }

        //! helper: sequential build from precomputed other slots
        template <typename S,typename U>
        inline void build_from(const    slots_of<S>                   &src,
                               typename type_traits<U>::parameter_type arg1)
        {
            assert(this->is_empty());
            if(size<src.size) resize_empty_to(src.size);
            try
            {
                for(size_t i=0;i<src.size;++i)
                {
                    append<S,U>(src[i],arg1);
                }
            }
            catch(...)
            {
                __free();
                throw;
            }
        }

        //! helper: sequential build from precomputed other slots
        template <typename S,typename U,typename V>
        inline void build_from(const    slots_of<S>                   &src,
                               typename type_traits<U>::parameter_type arg1,
                               typename type_traits<U>::parameter_type arg2)
        {
            assert(this->is_empty());
            if(size<src.size) resize_empty_to(src.size);
            try
            {
                for(size_t i=0;i<src.size;++i)
                {
                    append<S,U,V>(src[i],arg1,arg2);
                }
            }
            catch(...)
            {
                __free();
                throw;
            }
        }


        inline friend std::ostream & operator<<( std::ostream &os, const slots_of &s )
        {
            os << '[';
            for(size_t i=0;i<s.size;++i)
            {
                os << ' ' << s[i];
            }
            os << ' ' << ']';
            return os;
        }

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(slots_of);
        cslot         cmem;

    protected:
        inline void __swap_with( slots_of &other ) throw()
        {
            cswap_const(size,     other.size);
            cswap_const(capacity, other.capacity);
            cmem.swap_with(other.cmem);
        }

        inline void __free() throw()
        {
            size_t &n = (size_t&)size;
            while(n>0)
            {
                --n;
                destruct( static_cast<mutable_type *>(cmem.data)+n );
            }
        }

    };


    template <typename T>
    class dynamic_slots : public slots_of<T>
    {
    public:
        inline explicit dynamic_slots(size_t n=0) : slots_of<T>(n) {}
        inline virtual ~dynamic_slots() throw() {}
        inline void free() throw() { this->__free(); }
        inline void resize(size_t n) { free(); this->resize_empty_to(n); }
        
    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(dynamic_slots);
        
    };
    
    
}



#endif
