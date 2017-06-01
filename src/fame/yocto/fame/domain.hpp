#ifndef YOCTO_FAME_DOMAIN_INCLUDED
#define YOCTO_FAME_DOMAIN_INCLUDED 1

#include "yocto/fame/split.hpp"
#include "yocto/container/tuple.hpp"

namespace yocto
{
    namespace fame
    {
        
        template <typename COORD>
        class domain_of
        {
        public:
            YOCTO_FAME_DECL_COORD;
            
            YOCTO_TRIPLE_DECL(YOCTO_TUPLE_TEMPLATE,peer,
                              const coord1d,rank,
                              const_coord,ranks,
                              bool,local);
            YOCTO_TUPLE_END();
            
            
            typedef layout_of<COORD> layout_type;
            typedef split<COORD>     split_type;
            
            split_type full;
            const peer self;
            
            inline explicit domain_of(const coord1d      user_rank,
                                      const coord1d      user_size,
                                      const_coord       *user_cpus,
                                      const layout_type &user_full,
                                      param_coord        pbc) :
            full(user_full,user_size,user_cpus),
            self(user_rank,full.local_ranks(user_rank),false)
            {
                std::cerr << "sizes=" << full.sizes << std::endl;
                std::cerr << "rmax =" << full.rmax  << std::endl;
                // building neighbors' list
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const coord1d local_size = __coord(full.sizes,dim);
                    const bool    periodic   = __coord(pbc,dim) != 0;
                    if(local_size>1)
                    {
                        {
                            const peer    next_peer( create_peer(next_rank1d,dim,pbc) );
                            std::cerr << "next(" << self << ")=" << next_peer << std::endl;
                        }
                        
                        {
                            const peer    prev_peer( create_peer(prev_rank1d,dim,pbc) );
                            std::cerr << "prev(" << self << ")=" << prev_peer << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "local" << std::endl;
                        // only one
                        if(periodic)
                        {
                            // will create 'local' neighbor
                        }
                        else
                        {
                            // do nothing
                        }
                        
                    }
                    
                }
            }
            
            inline domain_of(const domain_of &other) :
            full(other.full),
            self(other.self)
            {
            }
            
            
            inline virtual ~domain_of() throw()
            {
            }
            
            
            
        private:
            YOCTO_DISABLE_ASSIGN(domain_of);
            inline peer create_peer(coord1d (*proc)(const coord1d,const coord1d),
                                    const size_t dim,
                                    const_coord &pbc)
            {
                assert(proc);
                const coord1d local_rmax = __coord(full.rmax,dim);
                coord         ranks(self.ranks);
                coord1d      &local_rank = __coord(ranks,dim);
                assert(proc(local_rank,local_rmax)!=local_rank);
                local_rank = proc(local_rank,local_rmax);
                const coord1d rank = full.global_rank(ranks);
                return peer(rank,ranks,false);
            }
        };
    }
}

#endif

