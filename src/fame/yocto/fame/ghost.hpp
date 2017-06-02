#ifndef YOCTO_FAME_GHOST_INCLUDED
#define YOCTO_FAME_GHOST_INCLUDED 1

#include "yocto/fame/layouts.hpp"

namespace yocto
{
    namespace fame
    {
        
        typedef vector<coord1d> indices_type;
        
        class _ghost : public indices_type
        {
        public:
            const coord1d rank;  //!< target
            explicit _ghost(const coord1d r) throw();
            virtual ~_ghost() throw();
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(_ghost);
        };
        
        template <class COORD>
        class ghost_of : public _ghost
        {
        public:
            inline virtual ~ghost_of() throw()
            {
            }
            
            inline explicit ghost_of(const layouts_of<COORD>               &L,
                                     const typename domain_of<COORD>::peer &p) :
            _ghost(p.rank)
            {
                
            }
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghost_of);
        };
        
    }
}

#endif
