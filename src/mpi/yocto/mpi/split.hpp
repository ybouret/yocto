#ifndef YOCTO_MPI_SPLIT_INCLUDED
#define YOCTO_MPI_SPLIT_INCLUDED 1

#include "yocto/math/point3d.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/exception.hpp"
#include "yocto/sort/quick.hpp"
#include "yocto/code/rand.hpp"

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
        // 2D API
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

        class task
        {
        public:
            size_t items;
            size_t async;
            size_t lcopy;
            word_t indx;
            inline task() throw() : items(0), async(0), lcopy(0), indx(0) {}
            inline task(const task &other) throw() : items(other.items), async(other.async), lcopy(other.lcopy), indx(other.indx) {}
            inline virtual ~task() throw() {}

            inline void set_index(const size_t num, const size_t den)  throw()
            {
                indx = word_t(den) * word_t(items) + word_t(num) * word_t(async);
            }

        private:
            YOCTO_DISABLE_ASSIGN(task);
        };

        template <typename T>
        class task2d : public task
        {
        public:
            const point2d<T> sizes;
            const point2d<T> width;

            inline task2d(const point2d<T> &s,const point2d<T> &w) throw() :
            task(),
            sizes(s),
            width(w)
            {
            }

            inline task2d(const task2d &other) throw() : task(other), sizes(other.sizes), width(other.width) {}
            inline virtual ~task2d() throw() {}

            inline void set_from(const int rank) throw()
            {

                assert(rank>=0);
                assert(rank<=int(sizes.__prod()));
                assert(sizes.x>0);
                assert(sizes.y>0);

                point2d<T> offset(1,1);
                point2d<T> length(width);
                perform(rank,sizes,offset,length);
                items = size_t( length.__prod() );
                async = 0;
                lcopy = 0;
                if(sizes.x>1)
                {
                    async += size_t(length.y);
                }
                else
                {
                    lcopy += size_t(length.y);
                }
                if(sizes.y>1)
                {
                    async += size_t(length.x);
                }
                else
                {
                    lcopy += size_t(length.x);
                }
            }

            inline void max_from(const int size) throw()
            {
                assert(size==int(sizes.__prod()));
                items = async = lcopy = 0;
                task2d sub(*this);
                for(int rank=0;rank<size;++rank)
                {
                    sub.set_from(rank);
                    items = max_of(items,sub.items);
                    async = max_of(async,sub.async);
                    lcopy = max_of(lcopy,sub.lcopy);
                }
            }

            static int compare(const task2d &lhs, const task2d &rhs)
            {
                assert(lhs.width==rhs.width);
                if(lhs.indx<rhs.indx)
                {
                    return -1;
                }
                else
                {
                    if(rhs.indx<lhs.indx)
                    {
                        return 1;
                    }
                    else
                    {
                        const point2d<T> &w = lhs.width;
                        if(w.y>=w.x)
                        {
                            return int(rhs.sizes.y)-int(lhs.sizes.y);
                        }
                        else
                        {
                            return int(rhs.sizes.x)-int(lhs.sizes.x);
                        }
                        return 0;
                    }
                }
            }

            inline friend std::ostream & operator<<( std::ostream &os, const task2d &t)
            {
                os << t.sizes << " => " << t.indx << "\t(items=" << t.items << ",async=" << t.async << ",lcopy=" << t.lcopy << ")";
                return os;
            }

        private:
            YOCTO_DISABLE_ASSIGN(task2d);
        };



        template <typename T> static inline
        point2d<T> compute_sizes(const int         size,
                                 const point2d<T> &width)
        {

            assert(width.x>0);
            assert(width.y>0);

            std::cerr << std::endl << "splitting " << width << " on " << size << " domain" << plural_s(size) << std::endl;
            if(size<=1)
            {
                return point2d<T>(size,size);
            }

            const point2d<T>  seq(1,1);
            task2d<T>         all(seq,width); all.items = width.__prod();
            const point2d<T>  x_split(size,1);
            const point2d<T>  y_split(1,size);
            const point2d<T> *r_split = 0;
            if(width.y>=width.x)
            {
                std::cerr << "\treference: splitting along y" << std::endl;
                if(y_split.y>width.y) throw exception("mpi_split: too many domains, even for greatest dimension Y");
                r_split = &y_split;
            }
            else
            {
                std::cerr << "\treference: splitting along x" << std::endl;
                if(x_split.x>width.x) throw exception("mpi_split: too many domains, even for greatest dimension X");
                r_split = &x_split;
            }

            task2d<T> ref(*r_split,width);
            ref.max_from(size);

            const size_t alpha_num = all.items - ref.items;
            const size_t alpha_den = ref.async;
            //alpha_den *= 2;
            std::cerr << "\t\talpha=" << alpha_num << "/" << alpha_den << std::endl;
            all.set_index(alpha_num,alpha_den);
            ref.set_index(alpha_num,alpha_den);
            std::cerr << "\tall=" << all << " / ref=" << ref << std::endl;

            vector< task2d<T>  > tasks(size,as_capacity);
            vector< point2d<T> > sizes(size,as_capacity);

            for(int sx=1;sx<=size;++sx)
            {
                for(int sy=1;sy<=size;++sy)
                {
                    // find valid sizes
                    if(sx*sy!=size) continue;
                    const point2d<T> trial(sx,sy);
                    //std::cerr << "\t\t-- " << trial << std::endl;
                    task2d<T> sub(trial,width);
                    sub.max_from(size);
                    sub.set_index(alpha_num,alpha_den);
                    //std::cerr << "\t\t\tsub=" << sub << std::endl;
                    tasks.push_back(sub);
                    sizes.push_back(trial);
                }
            }
            c_shuffle(tasks(),sizes(),tasks.size());
            co_qsort(tasks,sizes,task2d<T>::compare);
            for(size_t i=1;i<=tasks.size();++i)
            {
                std::cerr << "\t\t" << tasks[i] << std::endl;
            }


#if 0
            task all( width.__prod() );
            task ref;
            if(width.y>=width.x)
            {
                std::cerr << "\treference: splitting along y" << std::endl;
                const point2d<T> sizes(1,size);
                if(sizes.y>width.y) throw exception("mpi_split: too many domains, even for greatest dimension Y");
                ref.max_from(size,sizes,width);
            }
            else
            {
                std::cerr << "\treference: splitting along x" << std::endl;
                const point2d<T> sizes(size,1);
                if(sizes.x>width.x) throw exception("mpi_split: too many domains, even for greatest dimension X");
                ref.max_from(size,sizes,width);
            }

            const size_t alpha_num = all.work - ref.work;
            const size_t alpha_den = ref.coms;
            std::cerr << "\t\talpha=" << alpha_num << "/" << alpha_den << std::endl;
            all.set_index(alpha_num,alpha_den);
            ref.set_index(alpha_num,alpha_den);
            std::cerr << "\tall=" << all << std::endl;
            std::cerr << "\tref=" << ref << std::endl;



            //__________________________________________________________________
            //
            // scan all partitions
            //__________________________________________________________________
            vector<task>         tasks(size,as_capacity);
            vector< point2d<T> > sizes(size,as_capacity);
            for(int sx=1;sx<=size;++sx)
            {
                for(int sy=1;sy<=size;++sy)
                {
                    // find valid sizes
                    if(sx*sy!=size) continue;
                    const point2d<T> trial(sx,sy);
                    std::cerr << "\t\t-- " << trial << std::endl;
                    task sub;
                    sub.max_from(size,trial,width);
                    sub.set_index(alpha_num,alpha_den);
                    std::cerr << "\t\t\tsub=" << sub << std::endl;
                    tasks.push_back(sub);
                    sizes.push_back(trial);
                }
            }


            c_shuffle(tasks(),sizes(),tasks.size());
            co_qsort(tasks,sizes,task::compare);
            for(size_t i=1;i<=tasks.size();++i)
            {
                std::cerr << sizes[i] << " => " << tasks[i] << std::endl;
            }
#endif

            return point2d<T>();

        }


        //______________________________________________________________________
        //
        //
        // 3D API
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
