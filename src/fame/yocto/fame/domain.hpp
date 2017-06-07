#ifndef YOCTO_FAME_DOMAIN_INCLUDED
#define YOCTO_FAME_DOMAIN_INCLUDED 1

#include "yocto/fame/split.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/sequence/slots.hpp"

namespace yocto
{
    namespace fame
    {

        //______________________________________________________________________
        //
        //! a domain, used to build topology
        //______________________________________________________________________
        template <typename COORD>
        class domain
        {
        public:
            YOCTO_FAME_DECL_COORD;

            //__________________________________________________________________
            //
            //! peer information about this and neigbors
            //__________________________________________________________________
            YOCTO_TRIPLE_DECL(YOCTO_TUPLE_TEMPLATE,peer,
                              const coord1d,rank,
                              const_coord,ranks,
                              bool,local);
            inline void run_hash( hashing::function &h ) const throw()
            {
                h.run_type(rank);
                h.run_type(ranks);
                h.run_type(local);
            }
            YOCTO_TUPLE_END();

            //______________________________________________________________________
            //
            //! holds a pair or peers
            //______________________________________________________________________
            class link
            {
            public:
                peer *next;
                peer *prev;
                inline  link() throw() : next(0), prev(0) {}
                inline ~link() throw() { clear(); }
                inline void set_next(const peer &p) { assert(0==next); next = new peer(p); }
                inline void set_prev(const peer &p) { assert(0==prev); prev = new peer(p); }

                inline link(const link &other) : next(0), prev(0)
                {
                    try
                    {
                        if(other.next) set_next( * other.next );
                        if(other.prev) set_prev( * other.prev );
                    }
                    catch(...)
                    {
                        clear();
                        throw;
                    }
                }

                inline void run_hash( hashing::function &H ) const throw()
                {
                    if(next) { next->run_hash(H); }
                    if(prev) { prev->run_hash(H); }
                }

            private:
                YOCTO_DISABLE_ASSIGN(link);
                inline void clear() throw()
                {
                    if(prev) { delete prev; prev=0; }
                    if(next) { delete next; next=0; }
                }

            };

            typedef layout<COORD> layout_type;
            typedef split<COORD>  split_type;

            split_type           full;  //!< used to split
            const peer           self;  //!< this information
            layout_type          inner; //!< for simulation
            slots_of<const link> links; //!< for topology
            const_coord          pbc;   //!< to keep track

            //! to debug
            inline void run_hash( hashing::function &h ) const throw()
            {
                full.run_hash(h);
                self.run_hash(h);
                inner.run_hash(h);
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    links[dim].run_hash(h);
                }
            }

            inline explicit domain(const coord1d      user_rank,
                                   const coord1d      user_size,
                                   const_coord       *user_cpus,
                                   const layout_type &user_full,
                                   param_coord        user_pbc) :
            full(user_full,user_size,user_cpus),
            self(user_rank,full.local_ranks(user_rank),true),
            inner( full(self.rank) ),
            links(DIMENSION),
            pbc(user_pbc)
            {
                for(size_t i=0;i<DIMENSION;++i)
                {
                    links.push_back();
                }

                //______________________________________________________________
                //
                //
                // building neighbors' list, per dimension
                //
                //______________________________________________________________
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const coord1d local_size = __coord(full.sizes,dim);
                    const bool    periodic   = __coord(pbc,dim) != 0;
                    if(local_size>1)
                    {
                        //______________________________________________________
                        //
                        // we have at least one neighbor, next or prev
                        //______________________________________________________
                        const coord1d rmax = __coord(full.rmax,dim);
                        {
                            //__________________________________________________
                            //
                            // building next neighbor
                            //__________________________________________________
                            coord    ranks(self.ranks);
                            coord1d &rank = __coord(ranks,dim);
                            if(periodic||rank<rmax)
                            {
                                assert(next_rank1d(rank,rmax)!=rank);
                                rank = next_rank1d(rank,rmax);
                                const coord1d global_rank = full.global_rank(ranks);
                                const peer p(global_rank,ranks,false);
                                ((link &)links[dim]).set_next(p);
                            }
                            else
                            {
                                //std::cerr << "next(" << self << ")=none" << std::endl;
                            }
                        }

                        {
                            //__________________________________________________
                            //
                            // building prev neihgbor
                            //__________________________________________________
                            coord    ranks(self.ranks);
                            coord1d &rank = __coord(ranks,dim);
                            if(periodic||rank>0)
                            {
                                assert(prev_rank1d(rank,rmax)!=rank);
                                rank = prev_rank1d(rank,rmax);
                                const coord1d global_rank = full.global_rank(ranks);
                                const peer p(global_rank,ranks,false);
                                ((link &)links[dim]).set_prev(p);
                            }
                            else
                            {
                                //std::cerr << "prev(" << self << ")=none" << std::endl;
                            }
                        }
                    }
                    else
                    {
                        //______________________________________________________
                        //
                        // only one
                        //______________________________________________________
                        if(periodic)
                        {
                            //__________________________________________________
                            //
                            // will create 'local' neighbor
                            //__________________________________________________
                            const peer p(self.rank,self.ranks,true);
                            //std::cerr << "next(" << self << ")=" << p << std::endl;
                            //std::cerr << "prev(" << self << ")=" << p << std::endl;
                            ((link &)links[dim]).set_next(p);
                            ((link &)links[dim]).set_prev(p);

                        }
                        else
                        {
                            //__________________________________________________
                            //
                            // do nothing
                            //__________________________________________________
                            //std::cerr << "next(" << self << ")=" << "none" << std::endl;
                            //std::cerr << "prev(" << self << ")=" << "none" << std::endl;
                        }

                    }

                }
            }

            inline domain(const domain &other) :
            full(other.full),
            self(other.self),
            inner(other.inner),
            links(DIMENSION),
            pbc(other.pbc)
            {
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    links.push_back(other.links[dim]);
                }
            }
            
            
            inline virtual ~domain() throw()
            {
            }
            
            
            
        private:
            YOCTO_DISABLE_ASSIGN(domain);
        };
    }
}

#endif

