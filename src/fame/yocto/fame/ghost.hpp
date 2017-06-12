#ifndef YOCTO_FAME_GHOST_INCLUDED
#define YOCTO_FAME_GHOST_INCLUDED 1

#include "yocto/fame/layout.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/sequence/slots.hpp"
#include "yocto/sequence/some-arrays.hpp"

namespace yocto
{
    namespace fame
    {
        
        typedef vector<coord1d>         indices_type;
        template <typename COORD> class layouts;

        //______________________________________________________________________
        //
        //! a ghost is a list of indices to be transeferred
        //______________________________________________________________________
        class ghost : public indices_type
        {
        public:
            const coord1d rank;  //!< target
            explicit ghost(const coord1d r,const size_t n);
            virtual ~ghost() throw();
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghost);
        };


        typedef array<coord1d>                        indices_array;
        typedef some_arrays<2,coord1d,memory::global> indices_arrays;

        class ghosts_pair : public indices_arrays
        {
        public:
            ~ghosts_pair() throw();
            const coord1d rank;
            ghosts_pair(const coord1d r, const size_t n);

            indices_array &outer;
            indices_array &inner;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_pair);
        };

        //______________________________________________________________________
        //
        //! one dimensional next/prev ghost
        //______________________________________________________________________
        class ghosts
        {
        public:
            ghosts() throw();
            ~ghosts() throw();
            ghost *next;
            ghost *prev;
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts);
            void clear() throw();
        };

        //______________________________________________________________________
        //
        //! a base class to store YOCTO_FAME_DIM_OF(COORD) ghosts
        //______________________________________________________________________
        template <class COORD>
        class ghosts_base : public slots_of<ghosts>
        {
        public:
            inline virtual ~ghosts_base() throw() {}
            inline explicit ghosts_base() :
            slots_of<ghosts>(YOCTO_FAME_DIM_OF(COORD))
            {
                for(size_t dim=0;dim<YOCTO_FAME_DIM_OF(COORD);++dim)
                {
                    (void)push_back();
                }
            }
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_base);
        };

        //______________________________________________________________________
        //
        //! implementation of ghosts, compute indices from outer layout
        //______________________________________________________________________
        template <class COORD>
        class ghosts_of : public ghosts_base<COORD>
        {
        public:
            YOCTO_FAME_DECL_COORD;
            inline virtual ~ghosts_of() throw() {}
            explicit ghosts_of(const layouts<COORD> &L);
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_of);
        };
        
    }
}

#endif
