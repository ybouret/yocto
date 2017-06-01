#ifndef YOCTO_SPADE_DOMAIN_INCLUDED
#define YOCTO_SPADE_DOMAIN_INCLUDED 1

#include "yocto/spade/field3d.hpp"
#include "yocto/spade/split.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/sequence/slots.hpp"

namespace yocto
{
    namespace spade
    {
        
        //______________________________________________________________________
        //
        //! base class, marker for size/rank
        //______________________________________________________________________
        class _Domain
        {
        public:
            const int size;
            const int rank;
            
            virtual ~_Domain() throw();
            static const char Name[];
            
            _Domain(const _Domain &) throw();
            
        protected:
            explicit _Domain(const int r,const int s) throw();
            
        private:
            YOCTO_DISABLE_ASSIGN(_Domain);
            
        };
        
        //______________________________________________________________________
        //
        // finding topology
        //______________________________________________________________________
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
                if( size != num_cpus )
                {
                    throw exception("%s: #cpus=%d != size=%d", _Domain::Name, num_cpus,size);
                }
                return ans;
                
            }
            else
            {
                // automatic splitting
                return Split::DispatchCPUs(full,size);
            }
        }
        
        //______________________________________________________________________
        //
        //! peer local ranks and global rank
        //______________________________________________________________________
        template <typename COORD>
        YOCTO_PAIR_DECL(YOCTO_TUPLE_TEMPLATE,DomainPeer,const COORD,ranks,const coord1D,rank);
        YOCTO_PAIR_END();
        
        //______________________________________________________________________
        //
        //! local topology
        //______________________________________________________________________
        template <typename COORD>
        class DomainPeers
        {
        public:
            YOCTO_SPADE_DECL_COORD;
            typedef DomainPeer<COORD>        PeerType;
            typedef slots_of<const PeerType> PeerSlots;
            static  const size_t             DIMENSION = sizeof(COORD)/sizeof(coord1D);
            
            const PeerType self;
            PeerSlots      next;
            PeerSlots      prev;

            inline ~DomainPeers() throw() {}
            inline  DomainPeers(const coord1D                     rank,
                                typename Split::For<COORD>::Type &split) :
            self(split.getRanks(rank),rank),
            next(DIMENSION),
            prev(DIMENSION)
            {
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    {
                        const_coord    ranks = split.next(self.ranks,dim);
                        const coord1D  rank  = split.getRank(ranks);
                        const PeerType peer(ranks,rank);
                        next.push_back(peer);
                    }
                    
                    {
                        const_coord    ranks = split.prev(self.ranks,dim);
                        const coord1D  rank  = split.getRank(ranks);
                        const PeerType peer(ranks,rank);
                        prev.push_back(peer);
                    }
                }
                
            }
            
            inline DomainPeers(const DomainPeers &other) throw() :
            self(other.self),
            next(DIMENSION),
            prev(DIMENSION)
            {
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    next.push_back(other.next[dim]);
                    prev.push_back(other.prev[dim]);
                }
            }
            
            
        private:
            YOCTO_DISABLE_ASSIGN(DomainPeers);
        };
        
        //______________________________________________________________________
        //
        //! setting topology
        //______________________________________________________________________
        template <
        typename COORD
        >
        class DomainInfo : public _Domain
        {
        public:
            YOCTO_SPADE_DECL_COORD;
            typedef typename Split::For<COORD>::Type splitter;
            typedef layout_of<COORD>                 layout_type;
            typedef DomainPeers<COORD>               peers_type;
            
            const_coord       sizes; //!< the compute engines per dim
            splitter          full;  //!< full layout to be splitted
            const_coord       ranks; //!< local ranks
            const layout_type part;  //!< simulation layout.
            const peers_type  peers; //!< topology
            
            inline virtual ~DomainInfo() throw() {}
            
            inline explicit DomainInfo(const layout_type &the_full,
                                       const int          the_rank,
                                       const int          the_size,
                                       const_coord       *the_cpus = NULL) :
            _Domain(the_rank,the_size),
            sizes( DomainComputeSizes<coord>(size,the_cpus,the_full) ),
            full(the_full,sizes),
            ranks( full.getRanks(rank) ),
            part(  full(rank) ),
            peers(rank,full)
            {
                
            }
            
            inline DomainInfo(const DomainInfo &other) throw():
            _Domain(other),
            sizes(other.sizes),
            full(other.full),
            ranks(other.ranks),
            part(other.part),
            peers(other.peers)
            {
            }
            
            
        private:
            YOCTO_DISABLE_ASSIGN(DomainInfo);
        };
        
        typedef DomainInfo<coord1D> DomainInfo1D;
        typedef DomainInfo<coord2D> DomainInfo2D;
        typedef DomainInfo<coord3D> DomainInfo3D;

        //! need to compute ghost with ranks !
        template <typename T,typename COORD>
        class Domain : public DomainInfo<COORD>, public  field_for<T,COORD>::type
        {
        public:
            YOCTO_SPADE_DECL_COORD;
            YOCTO_ARGUMENTS_DECL_T;
            typedef typename field_for<T,COORD>::type field_type;
            typedef layout_of<coord>                  layout_type;
            typedef ghosts_of<coord>                  ghosts_type;
            typedef DomainInfo<COORD>                 domain_info;
            
            inline virtual ~Domain() throw() {}
            
            inline explicit Domain(const domain_info &the_info,
                                   const string      &the_name,
                                   const ghosts_type &the_ghosts) :
            domain_info(the_info),
            field_type(the_name,this->part,the_ghosts)
            {
            }
            
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Domain);
        };
        
    }
}

#endif
