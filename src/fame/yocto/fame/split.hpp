#ifndef YOCTO_FAME_SPLIT_INCLUDED
#define YOCTO_FAME_SPLIT_INCLUDED 1

#include "yocto/fame/layout.hpp"
#include "yocto/mpi/split.hpp"

namespace yocto
{
    namespace fame
    {
        
        
        //! routines to split a layout
        template <typename COORD>
        class split : public layout<COORD>
        {
        public:
            YOCTO_FAME_DECL_COORD;
            typedef layout<COORD> layout_type;
            
            const_coord   sizes;
            const coord1d size;
            const_coord   rmax;
            
            inline void run_hash( hashing::function &h ) const throw()
            {
                this->layout_type::run_hash(h);
                h.run_type(sizes);
                h.run_type(size);
                h.run_type(rmax);
            }
            
            inline explicit split(const layout_type &full,
                                  const coord1d      size,
                                  const_coord       *cpus = NULL) :
            layout_type(full),
            sizes(ComputeSizes(full,size,cpus)),
            size(__coord_prod(sizes)),
            rmax(sizes)
            {
                // checking in any case
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    if( __coord(sizes,dim) > __coord(this->width,dim) )
                    {
                        throw exception("fame.split: too many domains in dimension #%d",int(dim));
                    }
                }
                __coord_add( (coord &)rmax, -1);
            }
            
            inline split(const split &other) throw() :
            layout_type(other),
            sizes(other.sizes),
            size(other.size),
            rmax(other.rmax)
            {
            }
            
                    
            
            inline virtual ~split() throw()
            {
            }
            
            inline layout_type operator()(const coord1d rank) const
            {
                assert(rank>=0);
                assert(rank<size);
                COORD _start = this->lower;
                COORD _width = this->width;
                mpi_split::perform(rank,sizes,_start,_width);
                COORD _final = _start+_width;
                __coord_add(_final,-1);
                return layout_type(_start,_final);
            }
            
            inline coord local_ranks(const coord1d rank) const throw()
            {
                assert(rank>=0);
                assert(rank<size);
                return mpi_split::local_ranks(rank,sizes);
            }
            
            inline coord1d global_rank(param_coord ranks) const throw()
            {
                return mpi_split::get_rank_of(ranks,sizes);
            }
            
            
        private:
            YOCTO_DISABLE_ASSIGN(split);
            static inline coord ComputeSizes(const layout_type &full,
                                             const coord1d      size,
                                             const_coord       *cpus)
            {
                if(cpus)
                {
                    //__________________________________________________________
                    //
                    // user choosen partition
                    //__________________________________________________________
                    const_coord ans = *cpus;
                    assert(size==__coord_prod(ans));
                    return ans;
                }
                else
                {
                    //__________________________________________________________
                    //
                    // find best partition according to parameters
                    //__________________________________________________________
                    return mpi_split::compute_sizes(size,full.width);
                }
            }
        };
        
        
        
    }
}

#endif
