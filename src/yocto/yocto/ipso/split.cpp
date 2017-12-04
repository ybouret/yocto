#include "yocto/ipso/split.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/sequence/list.hpp"
#include "yocto/core/list.hpp"
#include "yocto/core/node.hpp"
#include "yocto/mpl/natural.hpp"

////////////////////////////////////////////////////////////////////////////////
//
// 1D
//
////////////////////////////////////////////////////////////////////////////////
namespace yocto
{
    namespace ipso
    {

        coord1D split::in1D::computeCoresMap(const size_t  cores,
                                             const coord1D length,
                                             const coord1D /* pbc */) throw()
        {
            assert(length>0);
            if(cores<=1)
            {
                return 1;
            }
            else
            {
                return min_of<coord1D>(cores,length);
            }
        }
    }

}

////////////////////////////////////////////////////////////////////////////////
//
// 2D/3D
//
////////////////////////////////////////////////////////////////////////////////
namespace yocto
{
    namespace ipso
    {
        // common for 2D/3D
        YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,__Work,size_t,run,size_t,com);
        inline __Work() throw() : run(0), com(0) {}
        YOCTO_PAIR_END();

        typedef core::node_of<__Work>   Work;
        typedef core::list_of_cpp<Work> __Works;
        class Works : public __Works
        {
        public:
            inline explicit Works() throw() : __Works()
            {
            }

            inline virtual ~Works() throw()
            {
            }

            inline void add_unique(Work *w) throw()
            {
                assert(w!=NULL);
                for(const Work *sub = head;sub;sub=sub->next)
                {
                    if( *sub == *w )
                    {
                        delete w;
                        w = 0;
                        break;
                    }
                }
                if(w)
                {
                    push_back(w);
                }
            }

            inline void show() const
            {
                std::cerr << "\t\t/--------" << std::endl;
                for(const Work *sub = head;sub;sub=sub->next)
                {
                    std::cerr << "\t\t|" << *sub << std::endl;
                }
                std::cerr << "\t\t\\--------" << std::endl;
            }
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Works);
        };

        namespace
        {

            template <typename COORD> static inline
            coord1D computeComs(const COORD &com,
                                const COORD &pbc,
                                const COORD &ranks,
                                const COORD &ncore) throw()
            {
                std::cerr << "\tranks=" << ranks << std::endl;
                unit_t ans = 0;
                for(size_t i=0;i<sizeof(COORD)/sizeof(coord1D);++i)
                {
                    const coord1D ni = __coord(ncore,i);
                    if(ni>1)
                    {
                        const coord1D ci = __coord(com,i);
                        ans += ci;
                        if( __coord(pbc,i) || split::is_bulk( __coord(ranks,i), ni ) )
                        {
                            ans += ci;
                        }
                        std::cerr << "\t\t\t+com/" << i << "=" << __coord(com,i) << std::endl;
                    }
                    // else no splitting in this direction!
                }
                std::cerr << "\t\tcom=" << ans << std::endl;
                return ans;
            }


            class Part2D : public split::in2D, public Works
            {
            public:
                explicit Part2D(const coord2D &n,
                                const patch2D &p,
                                const coord2D &pbc) :
                split::in2D(n,p),
                Works()
                {
                    std::cerr << "Part2D(" << n <<  ")" << std::endl;
                    for(size_t rank=0;rank<cores;++rank)
                    {
                        const patch2D sub   = (*this)(rank);
                        const coord2D ranks = last_ranks;
                        Work          *w    = new Work();
                        const coord2D  com(sub.width.y,sub.width.x);
                        w->run  = sub.items;
                        w->com  = computeComs(com,pbc,ranks,ncore);

                        // adding
                        add_unique(w);
                    }
                }

                virtual ~Part2D() throw() {}

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Part2D);
            };

            class Part3D : public split::in3D, public Works
            {
            public:
                explicit Part3D(const coord3D &n,
                                const patch3D &p,
                                const coord3D &pbc) :
                split::in3D(n,p),
                Works()
                {
                    for(size_t rank=0;rank<cores;++rank)
                    {
                        const patch3D sub   = (*this)(rank);
                        const coord3D ranks = last_ranks;
                        Work          *w = new Work();
                        const coord3D com(sub.width.y*sub.width.z,
                                          sub.width.x*sub.width.z,
                                          sub.width.x*sub.width.y);
                        w->run  = sub.items;
                        w->com  = computeComs(com,pbc,ranks,ncore);

                        // adding
                        add_unique(w);
                    }
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Part3D);
            };
        }

        coord2D split::in2D::computeCoresMap(const size_t  cores,
                                             const coord2D length,
                                             const coord2D pbc) throw()
        {
            if(cores<=1)
            {
                return coord2D(1,1);
            }

            const patch2D p(coord2D(1,1),coord2D(length));

            //__________________________________________________________________
            //
            // Run and Com for 1x1
            //__________________________________________________________________
            const   coord1D Lx   = length.x;
            const   coord1D Ly   = length.y;
            coord1D         Run  = Lx*Ly;
            std::cerr  << "Run=" << Run << std::endl;

            //__________________________________________________________________
            //
            // Run and Com for 2x1
            //__________________________________________________________________
            const coord2D core2x1(2,1);
            Part2D        part2x1(core2x1,p,pbc);
            part2x1.show();

            const coord2D core1x2(1,2);
            Part2D        part1x2(core1x2,p,pbc);
            part1x2.show();

            std::cerr << std::endl << std::endl;
            return coord2D(1,1);
            //__________________________________________________________________
            //
            // Run and Com for 1x1
            //__________________________________________________________________


            const coord1D nmax(cores);
            coord2D       n;
            for(n.x=1;n.x<=nmax;++n.x)
            {
                for(n.y=1;n.y<=nmax;++n.y)
                {
                    if(1==n.x&&1==n.y) continue;
                    const coord1D ncpu = n.__prod();
                    if(ncpu>nmax) continue;
                    std::cerr << "n=" << n << std::endl;
                    Part2D part(n,p,pbc);
                    std::cerr << "works: " << std::endl;
                    for(const Work *w=part.head;w;w=w->next)
                    {
                        std::cerr << "\t" << *w << std::endl;
                    }
                    std::cerr << std::endl;
                }
            }

            return coord2D(1,1);
        }

        coord3D split::in3D::computeCoresMap(const size_t  cores,
                                             const coord3D length,
                                             const coord3D pbc) throw()
        {
            if(cores<=1)
            {
                return coord3D(1,1,1);
            }

            const patch3D p(coord3D(1,1,1),coord3D(length));

            //__________________________________________________________________
            //
            // Run and Com for 1x1
            //__________________________________________________________________
            const   coord1D Lx   = length.x;
            const   coord1D Ly   = length.y;
            coord1D         Run1 = Lx*Ly;
            coord1D         Com1 = 0;
            if(pbc.x) Com1 += (Ly+Ly);
            if(pbc.y) Com1 += (Lx+Lx);

            const coord1D nmax(cores);
            coord3D       n;
            for(n.x=1;n.x<=nmax;++n.x)
            {
                for(n.y=1;n.y<=nmax;++n.y)
                {
                    for(n.z=1;n.z<=nmax;++n.z)
                    {
                        if(1==n.x&&1==n.y&&1==n.z) continue;
                        const coord1D ncpu = n.__prod();
                        if(ncpu>nmax) continue;
                        std::cerr << "n=" << n << std::endl;
                        Part3D part(n,p,pbc);
                        std::cerr << "works: " << std::endl;
                        for(const Work *w=part.head;w;w=w->next)
                        {
                            std::cerr << "\t" << *w << std::endl;
                        }
                    }
                }
            }


            return coord3D(1,1,1);
        }

    }

}


