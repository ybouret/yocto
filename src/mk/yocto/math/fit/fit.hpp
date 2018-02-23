#ifndef YOCTO_MATH_FIT_INCLUDED
#define YOCTO_MATH_FIT_INCLUDED 1

#include "yocto/associative/db-entry.hpp"
#include "yocto/associative/labelled.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/math/fcn/derivatives.hpp"

namespace yocto
{
    namespace math
    {

        template <typename T>
        class Fit
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef array<T>                                 Array;
            typedef typename numeric<T>::parametric_function Function;

            // handling variables
            typedef db_entry<T>            Variable;
            typedef labelled<set,Variable> VarDB;
            class Variables : public VarDB
            {
            public:
                inline explicit Variables() throw() : VarDB() {}
                inline explicit Variables(size_t n) : VarDB(n,as_capacity) {}

                inline virtual ~Variables() throw() {}
                
                inline void     load( const Array &a )
                {
                    assert(a.size()==this->size());
                    const size_t n = a.size();
                    size_t       j = 1;
                    for(typename VarDB::iterator i=this->begin();j<=n;++i,++j)
                    {

                    }
                }
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Variables);
            };
        };

    }
}

#endif

