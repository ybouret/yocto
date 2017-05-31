#ifndef YOCTO_COMPARATOR_INCLUDED
#define YOCTO_COMPARATOR_INCLUDED 1

#include "yocto/type/args.hpp"

namespace yocto
{
    //! increasing order
    template<typename T>
    inline int __compare( T const &lhs, T const &rhs ) throw()
    {
        return (lhs < rhs) ? -1 : ( rhs < lhs ? 1 : 0 );
    }

    //! decreasing order
    template <typename T>
    inline int __compare_decreasing(  T const &lhs, T const &rhs ) throw()
    {
        return - __compare<T>(lhs,rhs);
    }

    //! increasing order, for libc functions
    template <typename T>
    inline int __compareC( const void *lhs, const void *rhs ) throw()
    {
        assert(lhs); assert(rhs);
        return __compare<T>( *(T*)lhs, *(T*)rhs );
    }

    //! decreasing order, for libc functions
    template <typename T>
    inline int __compareC_decreasing( const void *lhs, const void *rhs ) throw()
    {
        assert(lhs); assert(rhs);
        return __compare_decreasing<T>( *(T*)lhs, *(T*)rhs );
    }

    template <typename T,size_t N>
    inline int __lexicographic_inc_fwd(const void *lhs, const void *rhs) throw()
    {
        assert(lhs); assert(rhs);
        const T *L = (const T *)lhs;
        const T *R = (const T *)rhs;
        for(size_t i=0;i<N;++i)
        {
            const T &l = L[i];
            const T &r = R[i];
            if(l<r) return -1;
            if(r<l) return  1;
        }
        return 0;
    }
    
    template <typename T,size_t N>
    inline int __lexicographic_dec_fwd(const void *lhs, const void *rhs) throw()
    {
        assert(lhs); assert(rhs);
        const T *L = (const T *)lhs;
        const T *R = (const T *)rhs;
        for(size_t i=0;i<N;++i)
        {
            const T &l = L[i];
            const T &r = R[i];
            if(l<r) return  1;
            if(r<l) return -1;
        }
        return 0;
    }

    
    
    
    template <typename T,size_t N>
    inline int __lexicographic_inc_rev(const void *lhs, const void *rhs) throw()
    {
        assert(lhs); assert(rhs);
        const T *L = (const T *)lhs;
        const T *R = (const T *)rhs;
        for(int i=int(N)-1;i>=0;--i)
        {
            const T &l = L[i];
            const T &r = R[i];
            if(l<r) return -1;
            if(r<l) return  1;
        }
        return 0;
    }

    template <typename T,size_t N>
    inline int __lexicographic_dec_rev(const void *lhs, const void *rhs) throw()
    {
        assert(lhs); assert(rhs);
        const T *L = (const T *)lhs;
        const T *R = (const T *)rhs;
        for(int i=int(N)-1;i>=0;--i)
        {
            const T &l = L[i];
            const T &r = R[i];
            if(l<r) return   1;
            if(r<l) return  -1;
        }
        return 0;
    }
    

    //! default increasing order comparator class
    template <typename T>
    class comparator
    {
    public:
        YOCTO_ARGUMENTS_DECL_T;

        inline  comparator() throw() {}
        inline ~comparator() throw() {}

        inline int operator()( param_type lhs, param_type rhs ) const throw()
        {
            return __compare<T>(lhs,rhs);
        }

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(comparator);
    };


}

#endif
