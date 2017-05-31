#ifndef YOCTO_MPI_SPLIT_INCLUDED
#define YOCTO_MPI_SPLIT_INCLUDED 1

#include "yocto/math/point3d.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/exception.hpp"
#include "yocto/sort/quick.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/mpl/rational.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/arc.hpp"

#include <cmath>
#include <cstdlib>

namespace yocto
{
    struct mpi_split
    {
        typedef uint64_t word_t;
        //______________________________________________________________________
        //
        //
        // 1D API
        //
        //______________________________________________________________________
        template <typename T> static inline
        void perform(const int rank,
                     const int size,
                     T        &offset,
                     T        &length) throw()
        {
            assert(size>0);
            assert(rank<size);
            T todo(length/size);
            for(int r=1;r<=rank;++r)
            {
                length -= todo;
                offset += todo;
                todo    = length/(size-r);
            }
            length = todo;
        }
        
        //______________________________________________________________________
        //
        //
        // 2D API for ranks and split
        //
        //______________________________________________________________________
        
        //! rank = ranks.x + sizes.x * ranks.y;
        template <typename T> static inline
        point2d<T> local_ranks(const int         rank,
                               const point2d<T> &sizes) throw()
        {
            assert(rank>=0);
            assert(sizes.x>0);
            assert(sizes.y>0);
            assert(rank<sizes.x*sizes.y);
            const ldiv_t d  = ldiv(rank,sizes.x);
            const T      rx(d.rem);
            const T      ry(d.quot);
            return point2d<T>(rx,ry);
        }
        
        //! rank = ranks.x + sizes.x * ranks.y;
        template <typename T> static inline
        int get_rank_of(const point2d<T> &ranks,
                        const point2d<T> &sizes) throw()
        {
            assert(ranks.x>=0);
            assert(ranks.y>=0);
            assert(ranks.x<sizes.x);
            assert(ranks.y<sizes.y);
            return ranks.x + sizes.x * ranks.y;
        }
        
        template <typename T> static inline
        void perform(const int         rank,
                     const point2d<T> &sizes,
                     point2d<T>       &offset,
                     point2d<T>       &length) throw()
        {
            const point2d<T> ranks = local_ranks(rank,sizes);
            perform(ranks.x,sizes.x,offset.x,length.x);
            perform(ranks.y,sizes.y,offset.y,length.y);
        }
        
        
        //______________________________________________________________________
        //
        //
        // 3D API for ranks and split
        //
        //______________________________________________________________________
        
        //! rank = ranks.x + sizes.x * rank.y + sizes.x * sizes.y * rank.z;
        /**
         rank = ranks.x + sizes.x * (rank.y+sizes.y*rank.z)
         */
        template <typename T> static inline
        point3d<T> local_ranks(const int         rank,
                               const point3d<T> &sizes) throw()
        {
            assert(rank>=0);
            assert(sizes.x>0);
            assert(sizes.y>0);
            assert(sizes.z>0);
            assert(rank<sizes.x*sizes.y*sizes.z);
            const ldiv_t dx  = ldiv(rank,sizes.x);
            const T      rx(dx.rem);
            const ldiv_t dy  = ldiv(dx.quot,sizes.y);
            const T      ry(dy.rem);
            const T      rz(dy.quot);
            return point3d<T>(rx,ry,rz);
        }
        
        //!  rank = ranks.x + sizes.x * (rank.y+sizes.y*rank.z)
        template <typename T> static inline
        int get_rank_of(const point3d<T> &ranks,
                        const point3d<T> &sizes) throw()
        {
            assert(ranks.x>=0);
            assert(ranks.y>=0);
            assert(ranks.x<sizes.x);
            assert(ranks.y<sizes.y);
            assert(ranks.z<sizes.z);
            
            return ranks.x + sizes.x*(ranks.y + sizes.y*ranks.z);
        }
        
        template <typename T> static inline
        void perform(const int         rank,
                     const point3d<T> &sizes,
                     point3d<T>       &offset,
                     point3d<T>       &length) throw()
        {
            const point3d<T> ranks = local_ranks(rank,sizes);
            perform(ranks.x,sizes.x,offset.x,length.x);
            perform(ranks.y,sizes.y,offset.y,length.y);
            perform(ranks.z,sizes.z,offset.z,length.z);
        }
        
        //______________________________________________________________________
        //
        //
        // helpers
        //
        //______________________________________________________________________
        
        template <typename T, template <typename> class POINT >
        static inline
        POINT<T> compute_length(const int       rank,
                                const POINT<T> &sizes,
                                const POINT<T> &width)
        {
            POINT<T> length(width);
            POINT<T> offset;
            {
                T *v = (T *)&offset;
                for(size_t i=0;i<sizeof(POINT<T>)/sizeof(T);++i)
                    v[i] = 1;
            }
            perform(rank,sizes,offset,length);
            return length;
        }
        
#if 0
        template <typename T> static inline
        point2d<T> compute_length(const int         rank,
                                  const point2d<T> &sizes,
                                  const point2d<T> &width)
        {
            point2d<T>       offset(1,1);
            point2d<T>       length(width);
            perform<T>(rank,sizes,offset,length);
            return length;
        }
        
        class task
        {
        public:
            mpq items;
            mpq async;
            mpq lcopy;
            inline explicit task() : items(), async(), lcopy() {}
            inline virtual ~task() throw() {}
            inline task(const task &other) :
            items(other.items), async(other.async), lcopy(other.lcopy) {}
            
        private:
            YOCTO_DISABLE_ASSIGN(task);
        };
        
        template <typename T>
        class task2d : public task
        {
        public:
            const point2d<T> sizes;  //!< the partition
            const int        size;   //!< sizes.__prod()
            const point2d<T> width;  //!< the layout
            point2d<T>       offset; //!< the sub layout offset
            point2d<T>       length; //!< the sub layout length
            
            inline explicit task2d(const point2d<T> &s,
                                   const point2d<T> &w) :
            task(), sizes(s),
            size(sizes.__prod()),
            width(w),
            offset(),
            length()
            {
            }
            
            inline virtual ~task2d() throw()
            {
            }
            
            inline task2d(const task2d &other) : task(other),
            sizes(other.sizes),
            size(other.size),
            width(other.width),
            offset(other.offset),
            length(other.length)
            {}
            
            // compute sub-partition spec
            inline void set(const int rank)
            {
                assert(rank<size);
                offset = point2d<T>(1,1);
                length = width;
                perform(rank,size, offset, length);
                items = int(length.__prod());
                async = 0;
                lcopy = 0;
                if(sizes.x>1)
                {
                    async += int(length.y);
                }
                else
                {
                    lcopy += int(length.y);
                }
                
                if(sizes.y>1)
                {
                    async += int(length.x);
                }
                else
                {
                    lcopy += int(length.x);
                }
            }
            
            inline friend std::ostream & operator<<(std::ostream &os, const task2d &t)
            {
                os << "(items=" << t.items << ",async=" << t.async << ",lcopy=" << t.lcopy << ")";
                return os;
            }
            
            // find alpha such that the splitting is faster than indx_old
            inline mpq find_alpha(const mpq &indx)
            {
                int r=-1;
                mpq alpha;
                for(int rank=0;rank<size;++rank)
                {
                    set(rank);
                    const mpq atemp = (indx-items)/async;
                    if(r<0||atemp<alpha)
                    {
                        r     = rank;
                        alpha = atemp;
                    }
                    std::cerr << "\t\t." << rank << "=>" << *this << std::endl;
                }
                set(r);
                return alpha;
            }
            
            inline mpq find_indx(const mpq &alpha)
            {
                int r=-1;
                mpq indx;
                for(int rank=0;rank<size;++rank)
                {
                    set(rank);
                    const mpq itmp = items + alpha * async;
                    if(r<0||itmp<indx)
                    {
                        r=rank;
                        indx=itmp;
                    }
                }
                set(r);
                return indx;
            }
            
        private:
            YOCTO_DISABLE_ASSIGN(task2d);
        };
        
        template <typename T>
        static inline
        mpq find_alpha(const int              size,
                       const point2d<T>      &width,
                       const mpq             &indx)
        {
            bool found = false;
            mpq  alpha;
            for(int sx=1;sx<=size;++sx)
            {
                for(int sy=1;sy<=size;++sy)
                {
                    if(sx*sy!=size) continue;
                    const point2d<T> sizes(sx,sy);
                    std::cerr << "\tsizes=" << sizes << std::endl;
                    task2d<T>        q(sizes,width);
                    const mpq  atemp = q.find_alpha(indx);
                    if(!found || atemp<alpha)
                    {
                        found=true;
                        alpha=atemp;
                    }
                    
                }
            }
            return alpha;
        }
        
        template <typename T>
        static inline
        void find_task(const int         size,
                       const point2d<T> &width,
                       mpq              &indx,
                       const mpq        &alpha)
        {
            bool       found = false;
            point2d<T> split;
            for(int sx=1;sx<=size;++sx)
            {
                for(int sy=1;sy<=size;++sy)
                {
                    if(sx*sy!=size) continue;
                    const point2d<T> sizes(sx,sy);
                    task2d<T>        q(sizes,width);
                    const mpq        itmp = q.find_indx(alpha);
                    std::cerr << "sizes=" << sizes << " => " << itmp << std::endl;
                }
            }
            
        }
        
#endif
        
        template <typename T, template <typename> class POINT >
        class partition : public counted_object
        {
        public:
            typedef POINT<T>           point_t;
            typedef arc_ptr<partition> pointer;
            
            const point_t sizes; //!< partition sizes
            const point_t width; //!< original width
            const int     size;  //!< size product
            const mpq     full;  //!< width product

            int           rank;
            mpq           items;
            mpq           async;
            mpq           lcopy;
            mpq           alpha;
            mpq           cost;
            
            
            inline explicit partition(const point_t &s, const point_t &w) :
            sizes(s), width(w), size( sizes.__prod() ), full( width.__prod() ),
            rank(-1), items(), async(), lcopy(), cost()
            {
            }
            
            static inline
            void build2d( const int C, const point_t &w, sequence<pointer> &parts)
            {
                assert(C>=2);
                parts.free();
                for(int sx=1;sx<=C;++sx)
                {
                    for(int sy=1;sy<=C;++sy)
                    {
                        if(C!=sx*sy) continue;
                        const point_t s(sx,sy);
                        if(s.x>w.x) continue;
                        if(s.y>w.y) continue;
                        const pointer ptr(new partition(s,w) );
                        parts.push_back(ptr);
                    }
                }
                if(parts.size()<=0)
                    throw exception("no possible partitions"); // TODO: change
            }
            
            inline void reset() throw()
            {
                rank=-1;
                items.ldz();
                async.ldz();
                lcopy.ldz();
            }
            
            inline mpq set2d(const int r)
            {
                assert(r<size);
                try
                {
                    reset();
                    rank = r;
                    const point_t length = compute_length(r,sizes,width);
                    items = int(length.__prod());
                    if(sizes.x>1)
                    {
                        async += int(length.y);
                    }
                    else
                    {
                        lcopy += int(length.y);
                    }
                    
                    if(sizes.y>1)
                    {
                        async += int(length.x);
                    }
                    else
                    {
                        lcopy += int(length.x);
                    }
                    
                }
                catch(...)
                {
                    reset();
                    throw;
                }
                std::cerr << "\t." << rank << "(items=" << items << ",async=" << async << ",lcopy=" << lcopy <<")" << std::endl;
                return (full-items)/async;
            }
            
            //! find alpha such that all compute time < full
            inline void  set_alpha2d()
            {
                alpha.ldz();
                bool found = false;
                for(int r=0;r<size;++r)
                {
                    const mpq atemp = set2d(r);
                    if(!found||atemp<alpha)
                    {
                        found = true;
                        alpha = atemp;
                    }
                }
            }
            
            inline void cost2d()
            {
                cost.ldz();
                for(int r=0;r<size;++r)
                {
                    set2d(r);
                    const mpq ctmp = items + alpha * async;
                    if(ctmp>cost) cost = ctmp;
                }
            }
            
            inline friend std::ostream & operator<<( std::ostream &os, const partition &p )
            {
                os << "#" << p.size << ":" << p.sizes << "/" << p.width;
                return os;
            }
            
            inline static int compare_alpha( const pointer &lhs, const pointer &rhs ) throw()
            {
                return __compare<mpq>(lhs->alpha,rhs->alpha);
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(partition);
        };
        
        template <typename T> static inline
        point2d<T> compute_sizes(const int         size,
                                 const point2d<T> &width)
        {
            
            assert(width.x>0);
            assert(width.y>0);
            typedef partition<T,point2d>     part_t;
            typedef typename part_t::pointer part_p;
            
            //__________________________________________________________________
            //
            // start algorithm, get rid of trivial cases
            //__________________________________________________________________
            std::cerr << "splitting " << width << " on " << size << " domain" << plural_s(size) << " => ";
            if(size<=1)
            {
                std::cerr << "no split" << std::endl;
                return point2d<T>(size,size);
            }
            std::cerr << std::endl;
            vector<part_p> parts(size,as_capacity);
            part_t::build2d(size,width,parts);
            for(size_t i=1;i<=parts.size();++i)
            {
                std::cerr << parts[i] << std::endl;
                parts[i]->set_alpha2d();
                std::cerr << "\t\talpha=" << parts[i]->alpha << std::endl;
                //const mpq &cost = parts[i]->compute_cost(
            }
            std::cerr << "SORTING/ALPHA" << std::endl;
            quicksort(parts,part_t::compare_alpha);
            const mpq alpha = parts[1]->alpha;
            std::cerr << "\tALPHA=" << alpha.to_double() << std::endl;
            for(size_t i=1;i<=parts.size();++i)
            {
                parts[i]->alpha = alpha;
                parts[i]->cost2d();
                std::cerr << parts[i] << std::endl;
                std::cerr << "\t\tcost=" << parts[i]->cost << std::endl;
            }
            
            return point2d<T>();
        }
        
        
        
        
        template <typename T> static inline
        point3d<T> compute_sizes(const int         size,
                                 const point3d<T> &width)
        {
            assert(width.x>0);
            assert(width.y>0);
            assert(width.z>0);
            
            std::cerr << "splitting " << width << " on " << size << " domain" << plural_s(size) << std::endl;
            
            //__________________________________________________________________
            //
            // find all partitions
            //__________________________________________________________________
            for(int sx=1;sx<=size;++sx)
            {
                const bool async_x = (sx>1);
                for(int sy=1;sy<=size;++sy)
                {
                    const bool async_y = (sy>1);
                    for(int sz=1;sz<=size;++sz)
                    {
                        // find valid sizes
                        if(sx*sy*sz!=size) continue;
                        const bool       async_z = (sz>1);
                        const point3d<T> sizes(sx,sy,sz);
                        std::cerr << "-- " << sizes << std::endl;
                        // loop over all ranks
                        size_t max_work = 0;
                        size_t num_coms = 0;
                        for(int rank=0;rank<size;++rank)
                        {
                            point3d<T> offset(1,1,1);
                            point3d<T> length(width);
                            perform(rank,sizes,offset,length);
                            const size_t local_work = size_t(length.__prod());
                            max_work  = max_of(local_work,max_work);
                            if(async_x)
                            {
                                num_coms += size_t(length.y)*size_t(length.z);
                            }
                            if(async_y)
                            {
                                num_coms += size_t(length.x)*size_t(length.z);
                            }
                            if(async_z)
                            {
                                num_coms += size_t(length.x)*size_t(length.y);
                            }
                        }
                        std::cerr << "\tmax_work = " << max_work << std::endl;
                        std::cerr << "\tnum_coms = " << num_coms << std::endl;
                    }
                }
            }
            return point3d<T>();
        }
        
        
    };
}

#endif
