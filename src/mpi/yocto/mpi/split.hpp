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
            
            //__________________________________________________________________
            //
            //! gather all possible partitions, checking sizes/width
            //__________________________________________________________________
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
                    throw exception("split2d: no possible partitions"); // TODO: change
            }
            
            //__________________________________________________________________
            //
            //! gather all possible partitions, checking sizes/width
            //__________________________________________________________________
            static inline
            void build3d( const int C, const point_t &w, sequence<pointer> &parts)
            {
                assert(C>=2);
                parts.free();
                for(int sx=1;sx<=C;++sx)
                {
                    for(int sy=1;sy<=C;++sy)
                    {
                        for(int sz=1;sz<=C;++sz)
                        {
                            if(C!=sx*sy*sz) continue;
                            const point_t s(sx,sy,sz);
                            if(s.x>w.x) continue;
                            if(s.y>w.y) continue;
                            if(s.z>w.z) continue;
                            const pointer ptr(new partition(s,w) );
                            parts.push_back(ptr);
                        }
                    }
                }
                if(parts.size()<=0)
                    throw exception("no possible partitions"); // TODO: change
            }

            
            //__________________________________________________________________
            //
            //! reset local domain
            //__________________________________________________________________
            inline void reset() throw()
            {
                rank=-1;
                items.ldz();
                async.ldz();
                lcopy.ldz();
            }
          
            
            //__________________________________________________________________
            //
            //! compute local domain specs, 2D
            //__________________________________________________________________
            inline void set_domain2d(const int r)
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
            }
            
            //__________________________________________________________________
            //
            //! compute local domain specs, 3D
            //__________________________________________________________________
            inline void set_domain3d(const int r)
            {
                assert(r<size);
                try
                {
                    reset();
                    rank = r;
                    const point_t length = compute_length(r,sizes,width);
                    items = int(length.__prod());
                    {
                        const int yz = int(length.y*length.z);
                        if(sizes.x>1)
                        {
                            async += yz;
                        }
                        else
                        {
                            lcopy += yz;
                        }
                    }
                    
                    
                    {
                        const int xz = int(length.x*length.z);
                        if(sizes.y>1)
                        {
                            async += xz;
                        }
                        else
                        {
                            lcopy += xz;
                        }
                    }
                    
                    {
                        const int xy = int(length.x*length.y);
                        if(sizes.z>1)
                        {
                            async += xy;
                        }
                        else
                        {
                            lcopy += xy;
                        }
                    }
                    
                }
                catch(...)
                {
                    reset();
                    throw;
                }
            }

            
            //__________________________________________________________________
            //
            // after a set_domain
            //__________________________________________________________________
            inline mpq __alpha() const {  return (full-items)/async; }
            
            
            //__________________________________________________________________
            //
            //! find alpha such that all compute time < full, 2D
            //__________________________________________________________________
            inline void  set_alpha2d()
            {
                alpha.ldz();
                bool found = false;
                for(int r=0;r<size;++r)
                {
                    set_domain2d(r);
                    const mpq atemp = __alpha();
                    if(!found||atemp<alpha)
                    {
                        found = true;
                        alpha = atemp;
                    }
                }
            }
            
            //__________________________________________________________________
            //
            //! find alpha such that all compute time < full, 3D
            //__________________________________________________________________
            inline void  set_alpha3d()
            {
                alpha.ldz();
                bool found = false;
                for(int r=0;r<size;++r)
                {
                    set_domain3d(r);
                    const mpq atemp = __alpha();
                    if(!found||atemp<alpha)
                    {
                        found = true;
                        alpha = atemp;
                    }
                }
            }

            
            //__________________________________________________________________
            //
            //! find the highest cost, and associated domain, 2D
            //__________________________________________________________________
            inline void cost2d()
            {
                cost.ldz();
                int ropt=-1;
                for(int r=0;r<size;++r)
                {
                    (void)set_domain2d(r);
                    const mpq ctmp = items + alpha * async;
                    if(ctmp>cost)
                    {
                        cost = ctmp;
                        ropt = r;
                    }
                }
                // set associated domain
                assert(ropt>=0);
                assert(ropt<size);
                set_domain2d(ropt);
            }
            
            //__________________________________________________________________
            //
            //! find the highest cost, and associated domain, 3D
            //__________________________________________________________________
            inline void cost3d()
            {
                cost.ldz();
                int ropt=-1;
                for(int r=0;r<size;++r)
                {
                    (void)set_domain3d(r);
                    const mpq ctmp = items + alpha * async;
                    if(ctmp>cost)
                    {
                        cost = ctmp;
                        ropt = r;
                    }
                }
                // set associated domain
                assert(ropt>=0);
                assert(ropt<size);
                set_domain3d(ropt);
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
            
            inline static int compare_costs2d( const pointer &lhs, const pointer &rhs) throw()
            {
                // cost major
                if(lhs->cost<rhs->cost) return -1;
                if(rhs->cost<lhs->cost) return  1;
                
                // same cost, check lcopy
                assert(lhs->cost==rhs->cost);
                if(lhs->lcopy<rhs->lcopy) return -1;
                if(rhs->lcopy<lhs->lcopy) return  1;
                
                // same cost and and same lcopy, split along largest last dim
                assert(lhs->lcopy==rhs->lcopy);
                
                return __compare_decreasing(lhs->sizes.y,rhs->sizes.y);
            }
            
            inline static int compare_costs3d( const pointer &lhs, const pointer &rhs) throw()
            {
                // cost major
                if(lhs->cost<rhs->cost) return -1;
                if(rhs->cost<lhs->cost) return  1;
                
                // same cost, check lcopy
                assert(lhs->cost==rhs->cost);
                if(lhs->lcopy<rhs->lcopy) return -1;
                if(rhs->lcopy<lhs->lcopy) return  1;
                
                // same cost and and same lcopy, split along largest last dim
                assert(lhs->lcopy==rhs->lcopy);
                
                return __compare_decreasing(lhs->sizes.z,rhs->sizes.z);
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
            std::cerr << "splitting " << width << " on " << size << " domain" << plural_s(size) << ":" << std::endl;
            if(size<=1)
            {
                std::cerr << "no split" << std::endl;
                return point2d<T>(size,size);
            }
            
            //__________________________________________________________________
            //
            // compute partitions, and set alpha from domains
            //__________________________________________________________________
            vector<part_p> parts(size,as_capacity);
            part_t::build2d(size,width,parts);
            for(size_t i=1;i<=parts.size();++i)
            {
                //std::cerr << parts[i] << std::endl;
                parts[i]->set_alpha2d();
                //std::cerr << "\t\talpha=" << parts[i]->alpha << std::endl;
            }
            //std::cerr << "SORTING/ALPHA" << std::endl;
            quicksort(parts,part_t::compare_alpha);
            const mpq alpha = parts[1]->alpha;
            std::cerr << "\talpha=" << alpha.to_double() << std::endl;
            
            //__________________________________________________________________
            //
            // set alpha for partitions, and compute cost
            //__________________________________________________________________
            for(size_t i=1;i<=parts.size();++i)
            {
                parts[i]->alpha = alpha;
                parts[i]->cost2d();
            }
            c_shuffle(parts(),parts.size());
            quicksort(parts,part_t::compare_costs2d);
            for(size_t i=1;i<=parts.size();++i)
            {
                const part_t &p = *parts[i];
                std::cerr << p << " => " << parts[i]->cost.to_double() << " (items=" << p.items << ",async=" << p.async << ",lcopy=" << p.lcopy <<")" << std::endl;
            }
            return parts[1]->sizes;
        }
        
        
        
        
        template <typename T> static inline
        point3d<T> compute_sizes(const int         size,
                                 const point3d<T> &width)
        {
            assert(width.x>0);
            assert(width.y>0);
            assert(width.z>0);
            typedef partition<T,point3d>     part_t;
            typedef typename part_t::pointer part_p;
            //__________________________________________________________________
            //
            // start algorithm, get rid of trivial cases
            //__________________________________________________________________
            std::cerr << "splitting " << width << " on " << size << " domain" << plural_s(size) << ":" << std::endl;
            if(size<=1)
            {
                std::cerr << "no split" << std::endl;
                return point3d<T>(size,size,size);
            }
            
            //__________________________________________________________________
            //
            // compute partitions, and set alpha from domains
            //__________________________________________________________________
            vector<part_p> parts(size,as_capacity);
            part_t::build3d(size,width,parts);
            for(size_t i=1;i<=parts.size();++i)
            {
                //std::cerr << parts[i] << std::endl;
                parts[i]->set_alpha3d();
            }
            //std::cerr << "SORTING/ALPHA" << std::endl;
            quicksort(parts,part_t::compare_alpha);
            const mpq alpha = parts[1]->alpha;
            std::cerr << "\talpha=" << alpha.to_double() << std::endl;
            
            //__________________________________________________________________
            //
            // set alpha for partitions, and compute cost
            //__________________________________________________________________
            for(size_t i=1;i<=parts.size();++i)
            {
                parts[i]->alpha = alpha;
                parts[i]->cost3d();
            }
            c_shuffle(parts(),parts.size());
            quicksort(parts,part_t::compare_costs3d);
            for(size_t i=1;i<=parts.size();++i)
            {
                const part_t &p = *parts[i];
                std::cerr << p << " => " << parts[i]->cost.to_double() << " (items=" << p.items << ",async=" << p.async << ",lcopy=" << p.lcopy <<")" << std::endl;
            }
            return parts[1]->sizes;
        }
        
        
    };
}

#endif
