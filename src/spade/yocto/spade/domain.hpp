#ifndef YOCTO_SPADE_DOMAIN_INCLUDED
#define YOCTO_SPADE_DOMAIN_INCLUDED 1

#include "yocto/spade/field3d.hpp"
#include "yocto/spade/split.hpp"

namespace yocto
{
    namespace spade
    {
        
        class _Domain
        {
        public:
            const int size;
            const int rank;
            
            virtual ~_Domain() throw();
            static const char Name[];
            
            
        protected:
            explicit _Domain(const int r,const int s) throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(_Domain);
            
        };
        
        template <typename COORD> inline
        COORD DomainComputeSizes(const int               size,
                                 const COORD            *cpus,
                                 const layout_of<COORD> &full)
        {
            assert(size>0);
            if(cpus)
            {
                // user wants to set the topology
                COORD ans = *cpus;
                const int num_cpus = __coord_prod(ans);
                if( size == num_cpus )
                {
                    throw exception("%s: #cpus=%d != size=%d", _Domain::Name, num_cpus,size);
                }
                return ans;
                
            }
            else
            {
                // automatic splitting
                return Split::DispatchCPUS(full,size);
            }
        }
        
        
        

        template <
        typename COORD
        >
        class DomainInfo : public _Domain
        {
        public:
            YOCTO_SPADE_DECL_COORD;
            typedef typename Split::For<COORD>::Type splitter;
            typedef layout_of<COORD>                 layout_type;
            
            const_coord       sizes;
            splitter          full;
            const_coord       ranks;
            const layout_type part;
            
            inline virtual ~DomainInfo() throw() {}
            
            inline explicit DomainInfo(const layout_type &the_full,
                                       const int          the_rank,
                                       const int          the_size,
                                       const_coord       *the_cpus = NULL) :
            _Domain(the_rank,the_size),
            sizes( DomainComputeSizes(size,the_cpus,the_full) ),
            full(the_full,sizes),
            ranks( full.getRanks(rank) ),
            part(  full(rank) )
            {
                
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(DomainInfo);
            
           
        };
        
        
    }
}

#endif
