#include "yocto/ipso/split.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/core/list.hpp"
#include "yocto/core/node.hpp"
#include "yocto/mpl/rational.hpp"
#include "yocto/sort/merge.hpp"
#include "yocto/sort/network.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/code/alea.hpp"

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
        //! common for 2D/3D
        /**
         number of items to run and to com(municate)
         */
        YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,__Work,mpn,run,mpn,com);
        inline __Work() throw() : run(0), com(0) {}
        YOCTO_PAIR_END();

        //! make a node of the tuples
        typedef core::node_of<__Work>   Work;

        //! make a list of tuples
        typedef core::list_of_cpp<Work> __Works;
        class Works : public __Works
        {
        public:
            mpn tmx;
            inline explicit Works() throw() : __Works(), tmx(0)
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


            //! compute an integer timing coefficient
            inline void computeTmxWith(const mpq &beta)
            {
                const mpn   den = beta.den;
                const mpn   num = beta.num.n;
                const Work *w   = head;

                mpn res = (w->run+num*w->com)*den;
                for(w=w->next;w;w=w->next)
                {
                    const mpn tmp = (w->run+num*w->com)*den;
                    if(res<tmp)
                    {
                        res = tmp;
                    }
                }
                tmx = res;
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Works);
        };


        //! compute the number of items to communicate
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

                    // check is another connection is to be made
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

        template <typename PART> static inline
        int compareByTmx(const PART *lhs, const PART *rhs,void*) throw()
        {
            if(lhs->tmx<rhs->tmx)
            {
                return -1;
            }
            else
            {
                if(rhs->tmx<lhs->tmx)
                {
                    return 1;
                }
                else
                {
                    // same timings
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
                       const coord1D  nmax,
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

        class Part3D : public object, public split::in3D, public Works
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
                       const coord1D  nmax,
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

        template <const size_t DIM> void __netsort(mpq *);

        template <> void __netsort<2>(mpq *arr)
        {
            netsort<mpq>::level2(arr);
        }

        template <> void __netsort<3>(mpq *arr)
        {
            netsort<mpq>::level3(arr);
        }

        //! the DIM first partitions have 2 cores, only one way to split!
        /**
         compute the coefficient so that the worst work domain is faster than
         the full domain
         */
        template <typename PART>
        mpq computeComFactor(const typename PART::List &parts, const mpn &Run)
        {
            static const size_t DIM = sizeof(typename PART::Coord)/sizeof(coord1D);
            assert(parts.size>=DIM);
            const PART *p = parts.head;
            //std::cerr << "Run=" << Run << std::endl;
            vector<mpq> beta(DIM,as_capacity);
            for(size_t count=DIM;count>0;--count,p=p->next)
            {
                assert(1==p->size);
                assert(2==p->cores);
                const Work *w   = p->head;
                const mpn   den = Run-w->run;
                const mpq   fac = mpq(den,w->com);
                beta.push_back(fac);
                //std::cerr << "\trun=" << w->run << ", com=" << w->com << " => beta=" << beta.back() << std::endl;
            }
            __netsort<DIM>(beta());
            return beta[1];
        }

        template <typename PART>
        const PART  *computeTmxAndSort(typename PART::List &parts, const mpq &beta )
        {
            // compute the timing coef for each partition
            for(PART *part=parts.head;part;part=part->next)
            {
                part->computeTmxWith(beta);
            }

            // sort by increasing timing, then lexicographic cores
            core::merging<PART>::sort(parts,compareByTmx<PART>,NULL);

#if 0
            for(const PART *part = parts.head;part;part=part->next)
            {
                std::cerr << part->ncore << " => " << part->tmx << std::endl;
            }
#endif
            
            return parts.head;
        }



        coord2D split::in2D::computeCoresMap(const size_t  cores,
                                             const coord2D length,
                                             const coord2D pbc)
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

            //__________________________________________________________________
            //
            // compute partitions
            //__________________________________________________________________
            Part2D::List  parts;
            Part2D::Build(parts,cores,p,pbc);

            //__________________________________________________________________
            //
            // compute com ratio
            //__________________________________________________________________
            const mpq beta = computeComFactor<Part2D>(parts,Run);

            //__________________________________________________________________
            //
            // compute com ratio
            //__________________________________________________________________
            return computeTmxAndSort<Part2D>(parts,beta)->ncore;
            //return coord2D(1,1);
        }

        coord3D split::in3D::computeCoresMap(const size_t  cores,
                                             const coord3D length,
                                             const coord3D pbc)
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
            const   coord1D Lz   = length.z;
            const   mpn     Run  = Lx*Ly*Lz;


            Part3D::List parts;
            Part3D::Build(parts,cores,p,pbc);

            const mpq beta = computeComFactor<Part3D>(parts,Run);
            return computeTmxAndSort<Part3D>(parts,beta)->ncore;
        }

    }

}


