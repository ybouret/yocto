
#ifndef YOCTO_IPSO_BOX_INCLUDED
#define YOCTO_IPSO_BOX_INCLUDED 1

#include "yocto/ipso/vertex.hpp"

namespace yocto
{
    namespace ipso
    {

        template <typename T,const size_t DIM>
        class box : public object
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef vertex_for<type,DIM> __vertex;
            typedef typename __vertex::type  vtx;
            
            const vtx  lower;
            const vtx  upper;
            const vtx  width;
            const_type measure;

            inline virtual ~box() throw() {}

            inline explicit box(const vtx lo, const vtx up) throw() :
            lower(lo),
            upper(up),
            width(upper),
            measure(0)
            {
                setup();
            }

            
            inline box(const box &other) throw() :
            lower(other.lower),
            upper(other.upper),
            width(other.width),
            measure(other.measure)
            {
            }

            template <typename U,typename V>
            static inline box from(const typename vertex_for<U,DIM>::type lo,
                                   const typename vertex_for<V,DIM>::type up)
            {
                return box( __vertex::from(lo), __vertex::from(up) );
            }
            

        private:
            YOCTO_DISABLE_ASSIGN(box);
            inline void setup() throw()
            {
                type *l = (type *)&lower;
                type *u = (type *)&upper;
                type *w = (type *)&width;
                type &m = (type &)measure;
                m=type(1);
                for(size_t i=0;i<DIM;++i)
                {
                    if(u[i]<l[i])
                    {
                        cswap(u[i],l[i]);
                    }
                    assert(l[i]<=u[i]);
                    m *= (w[i] = u[i]-l[i]);
                }
            }
        };

    }
}

#endif

