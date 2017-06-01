#ifndef YOCTO_FAME_SPLIT_INCLUDED
#define YOCTO_FAME_SPLIT_INCLUDED 1

#include "yocto/fame/layout.hpp"
#include "yocto/mpi/split.hpp"

namespace yocto
{
    namespace fame
    {
        
        template <typename COORD>
        class split : public layout_of<COORD>
        {
        public:
            YOCTO_FAME_DECL_COORD;
            typedef layout_of<COORD> layout_type;
            
            const_coord   sizes;
            const coord1d size;
            
            inline explicit split(const layout_type &full,
                                  param_coord        user_sizes) :
            layout_type(full),
            sizes(user_sizes),
            size(__coord_prod(sizes))
            {
                for(int dim=0;dim<DIMENSION;++dim)
                {
                    if( __coord(sizes,dim) > __coord(this->width,dim) )
                    {
                        throw exception("fame.split: too many domains in dimension #%d",dim);
                    }
                }
            }
            
            inline explicit split(const layout_type &full, const size_t cpus) :
            layout_type(full),
            sizes( mpi_split::compute_sizes(cpus,full.width) ),
            size(__coord_prod(sizes))
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
        };
        
        
        
    }
}

#endif
