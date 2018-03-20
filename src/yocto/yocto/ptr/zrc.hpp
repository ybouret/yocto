#ifndef YOCTO_POINTER_ZRC_INCLUDED
#define YOCTO_POINTER_ZRC_INCLUDED 1

#include "yocto/type/args.hpp"
#include <iosfwd>

namespace yocto
{
    //! automatic references counting with possible zero
    /**
     T must have:
     - void   withhold() throw()
     - bool   liberate() throw()
     - size_t refcount() const throw()
     */

    template <typename T>
    class zrc_ptr
    {
    public:
        YOCTO_ARGUMENTS_DECL_T;
        inline explicit zrc_ptr( T *p ) : pointee_( (mutable_type*)p )
        {
            if(pointee_)
            {
                pointee_->withhold();
            }
        }

        inline ~zrc_ptr() throw()
        {
            if( pointee_ && pointee_->liberate() )
            {
                delete pointee_;
            }
        }

        inline zrc_ptr( const zrc_ptr &other ) throw() :
        pointee_( other.pointee_ )
        {
            if(pointee_)
            {
                pointee_->withhold();
            }
        }


        inline type & operator*()  throw()
        {
            assert(pointee_);
            return *pointee_;
        }

        inline const_type & operator*()  const throw()
        {
            assert(pointee_);
            return *pointee_;
        }

        inline type * operator->() throw()
        {
            assert(pointee_);
            return pointee_;
        }

        inline const_type * operator->() const throw()
        {
            assert(pointee_);
            return pointee_;
        }

        inline type       * __get() throw()       { return pointee_; }
        inline const_type * __get() const throw() { return pointee_; }
        inline bool is_valid() const throw() { return 0 != pointee_; }

        inline void swap_with( zrc_ptr &other ) throw()
        {
            mutable_type *tmp = other.pointee_;
            other.pointee_    = pointee_;
            pointee_          = tmp;
        }

        inline zrc_ptr & operator=( const zrc_ptr &other ) throw()
        {
            zrc_ptr tmp(other);
            swap_with(tmp);
            return *this;
        }

        inline friend std::ostream & operator<<( std::ostream &os, const zrc_ptr &p)
        {
            os << *p;
            return os;
        }

    private:
        mutable_type *pointee_;
    };
}

#endif
