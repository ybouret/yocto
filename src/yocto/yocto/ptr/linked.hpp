#ifndef YOCTO_PTR_LINKED_INCLUDED
#define YOCTO_PTR_LINKED_INCLUDED 1

#include "yocto/type/args.hpp"

namespace yocto
{

    //! a wrapper smart pointer for lists
    /**
     the only restriction is an assumed  no-throw copy constructor
     for the internal pointer
     */
    template <typename T,
    template <typename> class POINTER>
    class linked_ptr
    {
    public:
        YOCTO_ARGUMENTS_DECL_T;

        linked_ptr *next;
        linked_ptr *prev;

        //! pass to handle
        inline linked_ptr( const POINTER<T> &p ) throw():
        next(0), prev(0), handle(p) {}

        //! default destructor
        inline ~linked_ptr() throw() {}

        //! copy constructor
        inline linked_ptr( const linked_ptr &other) throw() :
        next(0), prev(0), handle(other.handle) {}

        inline type       & operator*() throw()       { return *handle; }
        inline const_type & operator*() const throw() { return *handle; }
        
        inline type       * __get() throw()       { return handle.__get(); }
        inline const_type * __get() const throw() { return handle.__get(); }

        inline type * operator->() throw()
        {
            type *p = handle.__get(); assert(p); return p;
        }

        inline const_type * operator->() const throw()
        {
            const_type *p = handle.__get(); assert(p); return p;
        }


    private:
        POINTER<T> handle;
        YOCTO_DISABLE_ASSIGN(linked_ptr);
    };

}

#endif
