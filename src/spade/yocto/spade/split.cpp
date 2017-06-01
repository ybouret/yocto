#include "yocto/spade/split.hpp"
#include "yocto/exception.hpp"
#include "yocto/mpi/split.hpp"

#include <cstdlib>

namespace yocto
{
    namespace spade
    {
        void Split:: Basic(const size_t size,
                           const size_t rank,
                           coord1D     &offset,
                           coord1D     &length)
        {
            mpi_split::perform(rank,size,offset,length);
            if(length<=0)
            {
                throw exception("spade.split: empty region for %u.%u", unsigned(size), unsigned(rank) );
            }
        }
        
        
        Layout1D Split:: __in1D(const size_t    size,
                                const size_t    rank,
                                const Layout1D &full)
        {
            coord1D offset = full.lower;
            coord1D length = full.width;
            Basic(size,rank,offset,length);
            const coord1D lower = offset;
            const coord1D upper = offset+length-1;
            return Layout1D(lower,upper);
        }
    }
}

namespace yocto
{
    namespace spade
    {
        
        Split::In1D:: In1D(const Layout1D &full, const coord1D ncpu):
        Layout1D(full),
        size(max_of<coord1D>(1,ncpu)),
        rmax( coord1D(size)-1 )
        {
            if(size>size_t(full.width))
                throw exception("spade.split.in1d: too many domains");
        }
        
        Split:: In1D:: ~In1D() throw()
        {
        }
        
        Layout1D  Split:: In1D:: operator()(const coord1D rank) const
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            coord1D offset = lower;
            coord1D length = width;
            Basic(size,rank,offset,length);
            return Layout1D(offset,offset+length-1);
        }
        
        coord1D Split:: In1D:: getRanks(const coord1D rank) const throw()
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            return rank;
        }
        
        coord1D  Split:: In1D:: getRank(const coord1D ranks) const throw()
        {
            assert(ranks>=0);
            assert(ranks<coord1D(size));
            return ranks;
        }

        
        Split:: In1D:: In1D( const In1D &other ) throw() :
        Layout1D(*this),
        size(other.size),
        rmax(other.rmax)
        {
            
        }
    }
}

namespace yocto
{
    namespace spade
    {
        
        Split::In2D:: In2D(const Layout2D &full,
                           const coord2D   cpus) :
        Layout2D(full),
        sizes(max_of<coord1D>(cpus.x,1),max_of<coord1D>(cpus.y,1)),
        size(sizes.x*sizes.y),
        rmax(sizes.x-1,sizes.y-1)
        {
            if(sizes.x>full.width.x) throw exception("spade.split.in2d: too many X domains");
            if(sizes.y>full.width.y) throw exception("spade.split.in2d: too many Y domains");
        }
        
        Split:: In2D:: ~In2D() throw()
        {
        }
        
        Split:: In2D:: In2D( const In2D &other ) throw() :
        Layout2D(*this),
        sizes(other.sizes),
        size(other.size),
        rmax(other.rmax)
        {
            
        }
        
        
        Layout2D Split::In2D:: operator()(const coord1D rank) const
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            coord2D       start  = lower;
            coord2D       final  = width;
            mpi_split::perform(rank, sizes, start, final);
            final += start;
            --final.x;
            --final.y;
            return Layout2D(start,final);
            
        }
        
        coord2D Split::In2D:: getRanks(const coord1D rank) const throw()
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            return mpi_split::local_ranks(rank,sizes);
        }
        
        coord1D Split:: In2D:: getRank(const coord2D ranks) const throw()
        {
            assert(ranks.x>=0);
            assert(ranks.y>=0);
            assert(ranks.x<sizes.x);
            assert(ranks.y<sizes.y);
            const coord1D rank = ranks.y * sizes.x + ranks.x;
            assert(rank>=0);
            assert(rank<coord1D(size));
            return rank;
        }

        
    }
    
}


namespace yocto
{
    namespace spade
    {
        Split::In3D::In3D(const Layout3D &full,
                          const coord3D   cpus) :
        Layout3D(full),
        sizes(max_of<coord1D>(cpus.x,1),max_of<coord1D>(cpus.y,1),max_of<coord1D>(cpus.z,1)),
        zcof(sizes.x*sizes.y),
        size(zcof*sizes.z),
        rmax(sizes.x-1,sizes.y-1,sizes.z-1)
        {
        }
        
        Split:: In3D:: ~In3D() throw()
        {
        }
        
        Split:: In3D:: In3D( const In3D &other ) throw() :
        Layout3D(*this),
        sizes(other.sizes),
        zcof(other.zcof),
        size(other.size),
        rmax(other.rmax)
        {
            
        }
        
        coord3D Split:: In3D:: getRanks(const coord1D rank) const throw()
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            return mpi_split::local_ranks(rank,sizes);
        }
        
        coord1D Split:: In3D:: getRank(const coord3D ranks) const throw()
        {
            assert(ranks.x>=0);
            assert(ranks.y>=0);
            assert(ranks.z>=0);
            assert(ranks.x<sizes.x);
            assert(ranks.y<sizes.y);
            assert(ranks.z<sizes.z);
            const coord1D rank = ranks.z * zcof + ranks.y * sizes.x + ranks.x;
            assert(rank>=0);
            assert(rank<coord1D(size));
            return rank;
        }
        

        
        Layout3D Split:: In3D:: operator()(const coord1D rank) const throw()
        {
            assert(rank>=0);
            assert(rank<coord1D(size));
            coord3D       start  = lower;
            coord3D       final  = width;
            mpi_split::perform(rank, sizes, start, final);
            final += start;
            --final.x;
            --final.y;
            --final.z;
            return Layout3D(start,final);
            
        }
        
        
    }
}


namespace yocto
{
    namespace spade
    {
        coord1D Split:: DispatchCPUs(const Layout1D &L, const size_t cpus)
        {
            assert(cpus>0);
            return cpus;
        }
        
        coord2D Split:: DispatchCPUs(const Layout2D &L, const size_t cpus)
        {
            assert(cpus>0);
            return mpi_split::compute_sizes(cpus,L.width);
        }
        
        coord3D Split:: DispatchCPUs(const Layout3D &L, const size_t cpus)
        {
            assert(cpus>0);
            return mpi_split::compute_sizes(cpus,L.width);
        }

    }
}
