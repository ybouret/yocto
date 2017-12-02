#include "yocto/ipso/split.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/sequence/list.hpp"

namespace yocto
{
    namespace ipso
    {

        coord1D split::in1D::computeCoresMap(const size_t cores, const coord1D length, const coord1D ) throw()
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

        YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,Work,size_t,run,size_t,com);
        inline Work() throw() : run(0), com(0) {}
        YOCTO_PAIR_END();

        namespace
        {

            typedef list<Work> Works;

            class Part2D : public split::in2D
            {
            public:
                Works works;
                explicit Part2D(const coord2D &n,
                                const patch2D &p,
                                const coord2D &pbc) :
                split::in2D(n,p)
                {
                    for(size_t rank=0;rank<cores;++rank)
                    {
                        const patch2D sub   = (*this)(rank);
                        const coord2D ranks = last_ranks;
                        Work          w;
                        w.run  = sub.items;
                        w.com += sub.width.x;
                        w.com += sub.width.y;
                        // using pbc

                        // adding
                        bool found = false;
                        for(Works::iterator i=works.begin();i!=works.end();++i)
                        {
                            if(*i==w)
                            {
                                found=true;
                                break;
                            }
                        }
                        if(!found) works.push_back(w);
                    }
                }

                virtual ~Part2D() throw() {}

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Part2D);
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
            coord1D         Run1 = Lx*Ly;
            coord1D         Com1 = 0;
            if(pbc.x) Com1 += (Ly+Ly);
            if(pbc.y) Com1 += (Lx+Lx);

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
                    std::cerr << "\tworks=" << part.works << std::endl;
                }
            }


            return coord2D(1,1);
        }

    }

}


