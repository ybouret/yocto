#ifndef YOCTO_SPADE_FIELD_INCLUDED
#define YOCTO_SPADE_FIELD_INCLUDED 1

#include "yocto/spade/layout.hpp"
#include "yocto/spade/ghosts.hpp"
#include "yocto/exception.hpp"
#include "yocto/string.hpp"

namespace yocto
{
    namespace spade
    {

        class field_info : public object
        {
        public:
            virtual ~field_info() throw();
            const string name;
            const size_t dimension;
            const size_t allocated;

            virtual size_t      items()     const throw() = 0;
            virtual size_t      item_size() const throw() = 0;

        protected:
            explicit field_info(const string &id, const size_t dim) throw();
            void set_allocated(const size_t n) throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_info);
        };

        template <typename COORD>
        class field_layouts : public field_info
        {
        public:
            YOCTO_SPADE_DECL_COORD();
            typedef layout_of<coord> layout_type;
            typedef Ghosts<coord>    ghosts_type;
            static const size_t      DIMENSION = layout_type::DIMENSION;

            const layout_type inner;
            const layout_type outer;
            const ghosts_type ghosts;

            inline virtual ~field_layouts() throw() {}
            virtual const void *address_of( param_coord ) const throw() = 0;
            virtual size_t      items() const throw() { return outer.items; }

        protected:
            inline explicit field_layouts(const string      &id,
                                          const layout_type &L,
                                          const ghosts_type &G) :
            field_info(id,DIMENSION),
            inner(L),
            outer(inner.lower-G.lower.size,inner.upper+G.upper.size),
            ghosts(G)
            {

                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const coord1D w = __coord(inner.width,dim);
                    if(__coord(G.lower.size,dim)>w)
                    {
                        throw exception("field%uD, dim#%u: lower ghost too big", unsigned(DIMENSION), unsigned(dim) );
                    }
                    if(__coord(G.upper.size,dim)>w)
                    {
                        throw exception("field%uD, dim#%u: upper ghost too big", unsigned(DIMENSION), unsigned(dim) );
                    }
                }
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_layouts);
        };

        typedef field_layouts<coord1D> field_layouts1D;
        typedef field_layouts<coord2D> field_layouts2D;
        typedef field_layouts<coord3D> field_layouts3D;


        template <typename T, typename COORD>
        class field_of : public field_layouts<COORD>
        {
        public:
            YOCTO_SPADE_DECL_COORD();
            YOCTO_ARGUMENTS_DECL_T;

            typedef layout_of<coord> layout_type;
            typedef Ghosts<coord>    ghosts_type;
            
            virtual size_t      item_size() const throw() { return sizeof(type); }

        protected:
            inline explicit field_of(const string      &id,
                                     const layout_type &L,
                                     const ghosts_type &G) :
            field_layouts<COORD>(id,L,G)
            {}

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_of);
        };

    }
}

#endif
