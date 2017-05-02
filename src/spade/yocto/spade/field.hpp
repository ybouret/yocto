#ifndef YOCTO_SPADE_FIELD_INCLUDED
#define YOCTO_SPADE_FIELD_INCLUDED 1

#include "yocto/spade/layout.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace spade
    {

        class field_info : public object
        {
        public:
            virtual ~field_info() throw();

            virtual size_t get_dimension() const throw() = 0;

        protected:
            explicit field_info() throw();


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_info);
        };

        template <typename T,typename COORD>
        class field_of : public layout_of<COORD>
        {
        public:
            YOCTO_SPADE_DECL_COORD();
            typedef layout_of<COORD> layout_type;

            const layout_type inner;


        protected:
            explicit field_of(param_coord lo,
                              param_coord hi) :
            layout_type(lo,hi),
            inner(*this)
            {
                compute_ghosts();
            }

            explicit field_of(param_coord lo,
                              param_coord up,
                              param_coord ghost_lo,
                              param_coord ghost_up) :
            layout_type(__coord_lower_of(lo,up)-__coord_abs(ghost_lo),__coord_upper_of(lo,up)+__coord_abs(ghost_up)),
            inner(lo,up)
            {
                compute_ghosts();
            }

            virtual size_t get_dimension() const throw() { return layout_type::DIMENSION; }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_of);
            inline void compute_ghosts()
            {
                std::cerr << "field" << get_dimension() << "D: ghosts:" << std::endl;
                const layout_type &outer = *this;
                std::cerr << "outer=" << outer << std::endl;
                std::cerr << "inner=" << inner << std::endl;
                for(size_t dim=0;dim<layout_type::DIMENSION;++dim)
                {
                    std::cerr << "\tdim=" << dim << std::endl;
                    const coord1D w = __coord(this->width,dim);
                    // lower pos
                    {
                        const coord1D outer_lo = __coord(outer.lower,dim);
                        const coord1D inner_lo = __coord(inner.lower,dim);
                        const coord1D ghost_lo = inner_lo - outer_lo;
                        std::cerr << "\t\touter_lo=" << outer_lo << std::endl;
                        std::cerr << "\t\tinner_lo=" << inner_lo << std::endl;
                        std::cerr << "\t\tghost_lo=" << ghost_lo << std::endl;
                        if(ghost_lo>w)
                        {
                            throw exception("spade.field%uD, dim=%u: too many lower ghosts for width", unsigned( get_dimension() ), unsigned(dim) );
                        }

                    }

                    // upper pos
                    {
                        const coord1D outer_up = __coord(outer.upper,dim);
                        const coord1D inner_up = __coord(inner.upper,dim);
                        const coord1D ghost_up = outer_up - inner_up;
                        std::cerr << "\t\touter_up=" << outer_up << std::endl;
                        std::cerr << "\t\tinner_up=" << inner_up << std::endl;
                        std::cerr << "\t\tghost_up=" << ghost_up << std::endl;
                        if(ghost_up>w)
                        {
                            throw exception("spade.field%uD, dim=%u: too many upper ghosts for width", unsigned( get_dimension() ), unsigned(dim) );
                        }


                    }


                }
            }

        };

        



    }
}

#endif
