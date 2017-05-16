#ifndef YOCTO_MPI_SPLIT_INCLUDED
#define YOCTO_MPI_SPLIT_INCLUDED 1

#include "yocto/math/point3d.hpp"
#include "yocto/code/utils.hpp"
#include <cmath>
#include <cstdlib>

namespace yocto
{
    struct mpi_split
    {
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

        template <typename T> static inline
        point2d<T> compute_sizes(const int         size,
                                 const point2d<T> &width)
        {
            assert(width.x>0);
            assert(width.y>0);
            std::cerr << "splitting " << width << " on " << size << " domain" << plural_s(size) << std::endl;

            //__________________________________________________________________
            //
            // find reference split
            //__________________________________________________________________
            size_t ref_work = 0;
            size_t ref_coms = 0;
            if(width.y>=width.x)
            {
                // ref case is split along y
                for(int rank=0;rank<size;++rank)
                {
                    T offset = 1;
                    T length = width.y;
                    perform(rank,size,offset,length);
                    ref_work = max_of(ref_work,size_t(length)*size_t(width.x));
                }
                if(size>1)
                {
                    ref_coms += size * size_t(width.x);
                }
                std::cerr << "reference: splitting along y" << std::endl;
            }
            else
            {
                // ref case is split along x
                for(int rank=0;rank<size;++rank)
                {
                    T offset = 1;
                    T length = width.x;
                    perform(rank,size,offset,length);
                    ref_work = max_of(ref_work,size_t(length)*size_t(width.y));
                }
                if(size>1)
                {
                    ref_coms += size * size_t(width.y);
                }
                std::cerr << "reference: splitting along x" << std::endl;
            }
            std::cerr << "ref_work=" << ref_work << ", ref_coms=" << ref_coms << std::endl;

            //__________________________________________________________________
            //
            // find all partitions
            //__________________________________________________________________
            for(int sx=1;sx<=size;++sx)
            {
                const bool async_x = (sx>1);
                for(int sy=1;sy<=size;++sy)
                {
                    // find valid sizes
                    if(sx*sy!=size) continue;
                    const bool       async_y = (sy>1);
                    const point2d<T> sizes(sx,sy);
                    std::cerr << "-- " << sizes << std::endl;

                    // loop over all ranks
                    size_t max_work = 0;
                    size_t num_coms = 0;
                    for(int rank=0;rank<size;++rank)
                    {
                        point2d<T> offset(1,1);
                        point2d<T> length(width);
                        perform(rank,sizes,offset,length);
                        const size_t local_work = size_t(length.__prod());
                        max_work  = max_of(local_work,max_work);
                        if(async_x)
                        {
                            num_coms += size_t(length.y);
                        }
                        if(async_y)
                        {
                            num_coms += size_t(length.x);
                        }
                    }
                    std::cerr << "\tmax_work = " << max_work << std::endl;
                    std::cerr << "\tnum_coms = " << num_coms << std::endl;
                }
            }
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
