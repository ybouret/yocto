#ifndef YOCTO_SPADE_FIELD_INCLUDED
#define YOCTO_SPADE_FIELD_INCLUDED 1

#include "yocto/spade/layout.hpp"
#include "yocto/spade/ghosts.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace spade
    {

        class field_info : public object
        {
        public:
            virtual ~field_info() throw();
            const size_t dimension;
            const size_t bytes;


        protected:
            explicit field_info(const size_t dim) throw();
            void set_bytes(const size_t n) throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_info);
        };

        template <typename T, typename COORD>
        class field_of : public field_info
        {
        public:
            YOCTO_SPADE_DECL_COORD();
            YOCTO_ARGUMENTS_DECL_T;
            typedef layout_of<coord> layout_type;
            typedef ghosts<coord>    ghosts_type;
            static const size_t      DIMENSION = layout_type::DIMENSION;

            const layout_type inner;
            const layout_type outer;

            virtual const void *address_of( param_coord ) const throw() = 0;
            
        protected:
            inline explicit field_of(const layout_type &L,
                                     const ghosts_type &G) :
            field_info(DIMENSION),
            inner(L),
            outer(inner.lower-G.lower.size,inner.upper+G.upper.size)
            {
                std::cerr << "field" << DIMENSION << "D:" << std::endl;
                std::cerr << "inner=" << inner << std::endl;
                std::cerr << "outer=" << outer << std::endl;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const coord1D w = __coord(inner.width,dim);
                    if(G.lower.size>w)
                    {
                        throw exception("field%uD, dim#%u: lower ghost too big", unsigned(DIMENSION), unsigned(dim) );
                    }
                    if(G.upper.size>w)
                    {
                        throw exception("field%uD, dim#%u: upper ghost too big", unsigned(DIMENSION), unsigned(dim) );
                    }
                }
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_of);
        };

    }
}

#endif
