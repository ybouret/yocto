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
            explicit _ghost(const coord1d r,const size_t n);
            virtual ~_ghost() throw();
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(_ghost);
        };
        
        template <class COORD>
        class ghosts_of
        {
        public:
            inline virtual ~ghosts_of() throw()
            {
            }
            
            explicit ghosts_of(const layouts_of<COORD> &L);

            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_of);
        };
        
    }
}

#endif
