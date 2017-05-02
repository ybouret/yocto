#ifndef YOCTO_SPADE_FIELD_INCLUDED
#define YOCTO_SPADE_FIELD_INCLUDED 1

#include "yocto/spade/layout.hpp"

namespace yocto
{
    namespace spade
    {

        class field_info : public object
        {
        public:
            virtual ~field_info() throw();
            const size_t owned_memory;
            
        protected:
            explicit field_info() throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_info);
        };

        template <typename COORD>
        class field_ghosts : public field_info
        {
        public:
            const COORD lower_ghosts;
            const COORD upper_ghosts;
            
            inline virtual ~field_ghosts() throw() {}
            
            inline explicit field_ghosts(const COORD &lo,const COORD &up) throw() :
            lower_ghosts( __coord_ge_zero<COORD>(lo) ),
            upper_ghosts( __coord_ge_zero<COORD>(up) )
            {
            }
            
            inline explicit field_ghosts() throw() :
            lower_ghosts( coord_info<COORD>::zero ),
            upper_ghosts( coord_info<COORD>::zero )
            {
            }
            
            inline  field_ghosts( const field_ghosts &other ) throw() :
            lower_ghosts( other.lower_ghosts ),
            upper_ghosts( other.upper_ghosts )
            {
            }
            
            static const field_ghosts none;
            
        private:
            YOCTO_DISABLE_ASSIGN(field_ghosts);
        };
        
        template <typename T,typename COORD>
        class  field_of :
        public field_ghosts<COORD>,
        public layout_of<COORD>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef layout_of<COORD>                     layout_type;
            typedef field_ghosts<COORD>                  ghosts_type;
            typedef typename    layout_type::coord       coord;
            typedef typename    layout_type::const_coord const_coord;
            typedef typename    layout_type::param_coord param_coord;
            static const size_t DIMENSION = layout_type::DIMENSION;

            inline virtual ~field_of() throw() {}

            const layout_type inner;

            // coordinate API


        protected:
            inline explicit field_of(param_coord lo,
                                     param_coord up) :
            ghosts_type(),
            layout_type(lo,up),
            inner(*this)
            {
            }
            
            inline explicit field_of(param_coord        lo,
                                     param_coord        up,
                                     const ghosts_type &g)
            {
                
            }
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_of);
        };

    }
}

#endif
