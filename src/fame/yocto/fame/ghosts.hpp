#ifndef YOCTO_FAME_GHOSTS_INCLUDED
#define YOCTO_FAME_GHOSTS_INCLUDED 1

#include "yocto/fame/layout.hpp"
#include "yocto/sequence/slots.hpp"
#include "yocto/sequence/some-arrays.hpp"

namespace yocto
{
    namespace fame
    {

        template <typename COORD> class layouts; //!< forward declaration

        typedef array<coord1d>                        ghost_type;    //!< an array of indices
        typedef some_arrays<2,coord1d,memory::global> ghosts_type;   //!< 2 arrays of indices

        //______________________________________________________________________
        //
        //! a pair of ghosts on one side, one dimension
        //______________________________________________________________________
        class ghosts_pair : public ghosts_type
        {
        public:
            virtual ~ghosts_pair() throw();
            explicit ghosts_pair(const coord1d r, const size_t n);

            const coord1d rank;
            ghost_type   &outer; //!< to be received
            ghost_type   &inner; //!< to be sent

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_pair);
        };


        //______________________________________________________________________
        //
        //! two ghosts_pair, one dimension
        //______________________________________________________________________
        class ghosts
        {
        public:
            ghosts() throw();
            ~ghosts() throw();

            ghosts_pair *prev;
            ghosts_pair *next;


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts);
            void cleanup() throw();
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
