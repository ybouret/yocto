#ifndef YOCTO_SPADE_SPLIT_INCLUDED
#define YOCTO_SPADE_SPLIT_INCLUDED 1

#include "yocto/spade/layout.hpp"

namespace yocto
{
    namespace spade
    {
        struct Split
        {
            static void Basic(const size_t size,
                              const size_t rank,
                              coord1D     &offset,
                              coord1D     &length);


            static Layout1D __in1D(const size_t    size,
                                   const size_t    rank,
                                   const Layout1D &full);

            template <typename COORD> static inline
            COORD __coord_next_rank(const COORD &ranks, const COORD &rmax, const size_t dim) throw()
            {
                COORD          ans(ranks);
                coord1D       &nxt = *( ((coord1D *)&ans )+dim);
                const coord1D  lim = *( ((coord1D *)&rmax)+dim);
                if(nxt>=lim) nxt=0; else ++nxt;
                return ans;
            }
            
            template <typename COORD> static inline
            COORD __coord_prev_rank(const COORD &ranks, const COORD &rmax, const size_t dim) throw()
            {
                COORD          ans(ranks);
                coord1D       &nxt = *( ((coord1D *)&ans )+dim);
                const coord1D  lim = *( ((coord1D *)&rmax)+dim);
                if(nxt<=0) nxt=lim; else --nxt;
                return ans;
            }
            
            class In1D : public Layout1D
            {
            public:
                const size_t  size;
                const coord1D rmax;
                
                explicit In1D(const Layout1D &full,
                              const coord1D   ncpu);
                virtual ~In1D() throw();

                Layout1D operator()(const coord1D rank) const;
                coord1D  getRanks(const coord1D rank) const throw();
                coord1D  getRank(const coord1D ranks) const throw();
                
                //! next rank from local ranks
                inline coord1D  next(const coord1D ranks,const size_t dim) const throw()
                {
                    assert(dim<=0);
                    return __coord_next_rank<coord1D>(ranks,rmax,dim);
                }
                
                //! prev rank from local ranks
                inline coord1D prev(const coord1D ranks,const size_t dim) const throw()
                {
                    assert(dim<=0);
                    return __coord_prev_rank<coord1D>(ranks,rmax,dim);
                }
                
                In1D(const In1D &other) throw();
                
            private:
                YOCTO_DISABLE_ASSIGN(In1D);
            };


            class In2D : public Layout2D
            {
            public:
                const coord2D sizes;
                const size_t  size;
                const coord2D rmax;
                
                explicit In2D(const Layout2D &full,
                              const coord2D   cpus);
                virtual ~In2D() throw();
                In2D(const In2D &other) throw();
                
                Layout2D operator()(const coord1D rank) const;

                // TODO: implement rank to x,y and reverse

                //! get ranks for rank<size
                coord2D getRanks(const coord1D rank) const throw();
                
                //! get rank from ranks
                coord1D getRank(const coord2D ranks) const throw();
                
                //! next rank from local ranks
                inline coord2D  next(const coord2D ranks,const size_t dim) const throw()
                {
                    assert(dim<=1);
                    return __coord_next_rank<coord2D>(ranks,rmax,dim);
                }
                
                //! next rank from local ranks
                inline coord2D  prev(const coord2D ranks,const size_t dim) const throw()
                {
                    assert(dim<=1);
                    return __coord_prev_rank<coord2D>(ranks,rmax,dim);
                }

                
            private:
                YOCTO_DISABLE_ASSIGN(In2D);
            };


            class In3D : public Layout3D
            {
            public:
                const coord3D sizes;
                const coord1D zcof;
                const size_t  size;
                const coord3D rmax;
                
                explicit In3D(const Layout3D &full,
                              const coord3D   cpus);
                virtual ~In3D() throw();
                In3D(const In3D &other) throw();

                //! get ranks for rank<size
                coord3D getRanks(const coord1D rank) const throw();

                //! get rank from ranks
                coord1D  getRank(const coord3D ranks) const throw();

                
                Layout3D operator()(const coord1D rank) const throw();

                //! next rank from local ranks
                inline coord3D  next(const coord3D ranks,const size_t dim) const throw()
                {
                    assert(dim<=2);
                    return __coord_next_rank<coord3D>(ranks,rmax,dim);
                }
                
                //! next rank from local ranks
                inline coord3D  prev(const coord3D ranks,const size_t dim) const throw()
                {
                    assert(dim<=2);
                    return __coord_prev_rank<coord3D>(ranks,rmax,dim);
                }

            private:
                YOCTO_DISABLE_ASSIGN(In3D);
            };

            static coord1D DispatchCPUs(const Layout1D &L, const size_t cpus);
            static coord2D DispatchCPUs(const Layout2D &L, const size_t cpus);
            static coord3D DispatchCPUs(const Layout3D &L, const size_t cpus);

            template <typename COORD>
            struct For;
            
        };
        
        template <>
        struct Split::For<coord1D>
        {
            typedef Split::In1D Type;
        };
        
        template <>
        struct Split::For<coord2D>
        {
            typedef Split::In2D Type;
        };
        
        template <>
        struct Split::For<coord3D>
        {
            typedef Split::In3D Type;
        };
        
        
    }
}


#endif
