#include "yocto/ipso/split.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/core/list.hpp"
#include "yocto/core/node.hpp"
#include "yocto/mpl/natural.hpp"
#include "yocto/sort/merge.hpp"

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
        YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,__Work,mpn,run,mpn,com);
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
                //std::cerr << "\t\t/--------" << std::endl;
                for(const Work *sub = head;sub;sub=sub->next)
                {
                    //std::cerr << "\t\t|" << *sub << std::endl;
                }
                //std::cerr << "\t\t\\--------" << std::endl;
            }
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Works);
        };


        template <typename COORD> static inline
        coord1D computeComs(const COORD &com,
                            const COORD &pbc,
                            const COORD &ranks,
                            const COORD &ncore) throw()
        {
            unit_t ans = 0;
            for(size_t i=0;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                const coord1D ni = __coord(ncore,i);
                if(ni>1)
                {
                    // parallel in dimension #i
                    const coord1D ci = __coord(com,i);
                    ans += ci; // at least
                    if( __coord(pbc,i) || split::is_bulk( __coord(ranks,i), ni ) )
                    {
                        ans += ci; // in other direction
                    }
                }
                // else no splitting in this direction!
            }
            return ans;
        }

        template <typename PART>
        static inline int compareByCores(const PART *lhs, const PART *rhs, void *) throw()
        {
            if(lhs->cores<rhs->cores)
            {
                return -1;
            }
            else
            {
                if(rhs->cores<lhs->cores)
                {
                    return 1;
                }
                else
                {
                    return PART::Coord::lexicompare(lhs->ncore,rhs->ncore);
                }
            }

        }

        class Part2D : public object, public split::in2D, public Works
        {
        public:
            typedef coord2D Coord;
            typedef core::list_of_cpp<Part2D> List;
            Part2D *next;
            Part2D *prev;
            explicit Part2D(const coord2D &n,
                            const patch2D &p,
                            const coord2D &pbc) :
            split::in2D(n,p),
            Works(),
            next(0),
            prev(0)
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

            static inline
            void Build(List          &parts,
                       const size_t   nmax,
                       const patch2D &p,
                       const coord2D &pbc)
            {
                coord2D       n;
                for(n.y=1;n.y<=nmax;++n.y)
                {
                    for(n.x=1;n.x<=nmax;++n.x)
                    {
                        if(1==n.x&&1==n.y) continue;
                        const coord1D ncpu = n.__prod();
                        if(ncpu>nmax) continue;
                        Part2D *part = new Part2D(n,p,pbc);
                        parts.push_back(part);
                    }
                }
                core::merging<Part2D>::sort(parts,compareByCores,NULL);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Part2D);
        };

        class Part3D : public split::in3D, public Works
        {
        public:
            typedef coord3D Coord;
            typedef core::list_of_cpp<Part3D> List;
            Part3D *next;
            Part3D *prev;

            explicit Part3D(const coord3D &n,
                            const patch3D &p,
                            const coord3D &pbc) :
            split::in3D(n,p),
            Works(),
            next(0),
            prev(0)
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

            static inline
            void Build(List          &parts,
                       const size_t   nmax,
                       const patch3D &p,
                       const coord3D &pbc)
            {
                coord3D       n;
                for(n.z=1;n.z<=nmax;++n.z)
                {
                    for(n.y=1;n.y<=nmax;++n.y)
                    {
                        for(n.x=1;n.x<=nmax;++n.x)
                        {
                            if(1==n.x&&1==n.y&&1==n.z) continue;
                            const coord1D ncpu = n.__prod();
                            if(ncpu>nmax) continue;
                            Part3D *part = new Part3D(n,p,pbc);
                            parts.push_back(part);
                        }
                    }
                }
                core::merging<Part3D>::sort(parts,compareByCores,NULL);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Part3D);
        };



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
            const   mpn     Run  = Lx*Ly;
            std::cerr  << "Run=" << Run << std::endl;

            //__________________________________________________________________
            //
            // compute partitions
            //__________________________________________________________________
            Part2D::List  parts;
            Part2D::Build(parts,cores,p,pbc);
            std::cerr << "study #" << parts.size << " partitions" << std::endl;
            for(const Part2D *part = parts.head;part;part=part->next)
            {
                std::cerr << "\tcores=" << part->cores << ", " << part->ncore << std::endl;
                for(const Work *w=part->head;w;w=w->next)
                {
                    std::cerr << "\t\t" << *w << std::endl;
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
            const   mpn     Run  = Lx*Ly;

            Part3D::List parts;
            Part3D::Build(parts,cores,p,pbc);


            return coord3D(1,1,1);
        }

    }

}


