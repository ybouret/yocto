#ifndef YOCTO_IPSO_SWAPS_INCLUDED
#define YOCTO_IPSO_SWAPS_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/sequence/vector.hpp"


namespace yocto
{
    namespace ipso
    {
        //! a swap is a list of offsets
        typedef vector<coord1D> swap;

        //! MPI style swaps, local or async
        class swaps : public object
        {
        public:
            typedef core::list_of_cpp<swaps> list;
            
            const swap    send;   //!< inner indices
            const swap    recv;   //!< outer indices
            const size_t  count;  //!< send.size() = recv.size() = count
            const size_t  source; //!< rank of source
            const size_t  target; //!< rank of target
            const coord1D layers; //!< number of extra layers
            swaps        *next;
            swaps        *prev;

            explicit swaps(const size_t source_rank,
                           const size_t target_rank,
                           const size_t some_layers) throw();
            virtual ~swaps() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(swaps);
        };


    }
}
#endif
