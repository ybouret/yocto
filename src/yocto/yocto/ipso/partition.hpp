#ifndef YOCTO_IPSO_PARTITION_INCLUDED
#define YOCTO_IPSO_PARTITION_INCLUDED 1

#include "yocto/ipso/domain.hpp"

namespace yocto
{
    namespace ipso
    {

        template <typename COORD>
        class partition : public domain<COORD>::list
        {
        public:
            typedef domain<COORD>              domain_type;
            typedef typename domain_type::list domains;

            explicit partition(const divider<COORD>  &full,
                               const size_t           ng,
                               const COORD            pbcs,
                               const bool             build=false) :
            domains()
            {
                for(size_t rank=0;rank<full.size;++rank)
                {
                    this->push_back( new domain_type(full,rank,ng,pbcs,build) );
                }
            }

            virtual ~partition() throw()
            {
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(partition);
        };

        typedef partition<coord1D> partition1D;
        typedef partition<coord2D> partition2D;
        typedef partition<coord3D> partition3D;
    }
}

#endif

