#ifndef YOCTO_CORE_NODE_INCLUDED
#define YOCTO_CORE_NODE_INCLUDED 1

#include "yocto/type/args.hpp"

namespace yocto
{
    namespace core
    {
        template <typename T>
        class node_of : public T
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            node_of *next;
            node_of *prev;

            //! check sanity on dtor
            inline virtual ~node_of() throw() { assert(!next);assert(!prev); }

            //! default constructor
            inline explicit node_of() : T(), next(0), prev(0) {}

            //! copy constructor
            inline explicit node_of(const node_of &other) :  T(other), next(0), prev(0) {}

            //! object copy constructor
            inline explicit node_of(param_type     args)  :  T(args), next(0), prev(0) {}

            //! mixed object copy constructor
            template <typename U>
            inline explicit node_of(typename type_traits<U>::parameter_type args) : T(args), next(0), prev(0) {}

            template <typename U,typename V>
            inline explicit node_of(typename type_traits<U>::parameter_type argU,
                                    typename type_traits<V>::parameter_type argV) : T(argU,argV), next(0), prev(0) {}


        private:
            YOCTO_DISABLE_ASSIGN(node_of);
        };
        
        template <typename T>
        class dnode_of
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            dnode_of *next;
            dnode_of *prev;
            type      data;
            inline  ~dnode_of() throw() { assert(!next); assert(!prev); }
            inline   dnode_of() : next(0), prev(0), data() {}
            inline   dnode_of(const dnode_of &other) : next(0), prev(0), data(other.data) {}
            inline   dnode_of(param_type args) : next(0), prev(0), data(args)       {}
            
            YOCTO_MAKE_OBJECT
        private:
            YOCTO_DISABLE_ASSIGN(dnode_of);
        };
    }
}

#endif

