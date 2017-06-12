#include "yocto/fame/layouts.hpp"

namespace yocto
{
    namespace fame
    {
        ghost::ghost(const coord1d r, const size_t n):
        indices_type(n,as_capacity),
        rank(r)
        {
        }

        ghost:: ~ghost() throw() {}

    }
}

namespace yocto
{
    namespace fame
    {
        ghosts_pair:: ~ghosts_pair() throw()
        {
        }


        ghosts_pair:: ghosts_pair(const coord1d r, const size_t n) :
        indices_arrays(),
        rank(r),
        outer( next_array() ),
        inner( next_array() )
        {
            allocate(n);
        }

    }
}


namespace yocto
{
    namespace fame
    {
        ghosts::  ghosts() throw() : next(0), prev(0) {}
        ghosts:: ~ghosts() throw()
        {
            clear();
        }

        void ghosts:: clear() throw()
        {
            if(prev) { delete prev; prev=0; }
            if(next) { delete next; next=0; }
        }


        template <>
        ghosts_of<coord1d>:: ghosts_of( const layouts<coord1d> &L )
        {
            //std::cerr << "Ghost1D" << std::endl;
            const domain<coord1d>::link &xlnk = L.links[0];
            const size_t ng = L.depth;
            ghosts &G = (*this)[0];
            if(xlnk.prev)
            {

                ghost &g = * (G.prev = new ghost(xlnk.prev->rank,ng));
                for(coord1d x=L.outer.lower;x<L.inner.lower;++x)
                {
                    const coord1d v = L.outer.offset_of(x);
                    //std::cerr << "prev@" << x << " => " << v <<std::endl;
                    g.push_back(v);
                }
                assert(g.size()==ng);
            }
            if(xlnk.next)
            {
                ghost &g = * (G.next = new ghost(xlnk.next->rank,ng));
                for(coord1d x=L.inner.upper+1;x<=L.outer.upper;++x)
                {
                    const coord1d v = L.outer.offset_of(x);
                    //std::cerr << "next@" << x << " => " << v <<std::endl;
                    g.push_back(v);
                }
                assert(g.size()==ng);
            }
        }

        template <>
        ghosts_of<coord2d>:: ghosts_of( const layouts<coord2d> &L )
        {
            //std::cerr << "Ghost2D" << std::endl;
            const layout<coord2d> &outer = L.outer;
            const layout<coord2d> &inner = L.inner;

            //__________________________________________________________________
            //
            // X
            //__________________________________________________________________
            {
                const domain<coord2d>::link &xlnk = L.links[0];
                const size_t                     ng  = L.depth*outer.width.y;
                ghosts &G = (*this)[0];
                if(xlnk.prev)
                {
                    ghost &g = * (G.prev = new ghost(xlnk.prev->rank,ng));
                    for(coord1d x=outer.lower.x;x<inner.lower.x;++x)
                    {
                        for(coord1d y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            const coord2d q(x,y);
                            const coord1d v = outer.offset_of(q);
                            //std::cerr << "xprev@" << q << " => " << v <<std::endl;
                            g.push_back(v);
                        }
                    }
                    assert(g.size()==ng);
                }

                if(xlnk.next)
                {
                    ghost &g = * (G.next = new ghost(xlnk.next->rank,ng));
                    for(coord1d x=inner.upper.x+1;x<=outer.upper.x;++x)
                    {
                        for(coord1d y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            const coord2d q(x,y);
                            const coord1d v = outer.offset_of(q);
                            //std::cerr << "xnext@" << q << " => " << v <<std::endl;
                            g.push_back(v);
                        }
                    }
                    assert(g.size()==ng);
                }
            }


            //__________________________________________________________________
            //
            // Y
            //__________________________________________________________________
            {
                const domain<coord2d>::link &ylnk = L.links[1];
                const size_t                     ng  = L.depth*outer.width.x;
                ghosts &G = (*this)[1];
                if(ylnk.prev)
                {
                    ghost &g = *( G.prev = new ghost(ylnk.prev->rank,ng));
                    for(coord1d x=outer.lower.x;x<=outer.upper.x;++x)
                    {
                        for(coord1d y=outer.lower.y;y<inner.lower.y;++y)
                        {
                            const coord2d q(x,y);
                            const coord1d v = outer.offset_of(q);
                            //std::cerr << "yprev@" << q << " => " << v <<std::endl;
                            g.push_back(v);
                        }
                    }
                    assert(g.size()==ng);
                }

                if(ylnk.next)
                {
                    ghost &g = *( G.next = new ghost(ylnk.next->rank,ng));
                    for(coord1d x=outer.lower.x;x<=outer.upper.x;++x)
                    {
                        for(coord1d y=inner.upper.y+1;y<=outer.upper.y;++y)
                        {
                            const coord2d q(x,y);
                            const coord1d v = outer.offset_of(q);
                            //std::cerr << "ynext@" << q << " => " << v <<std::endl;
                            g.push_back(v);
                        }
                    }
                    assert(g.size()==ng);
                }

            }
        }


        template <>
        ghosts_of<coord3d>:: ghosts_of( const layouts<coord3d> &L )
        {
            //std::cerr << "Ghost3D" << std::endl;
            const layout<coord3d> &outer = L.outer;
            const layout<coord3d> &inner = L.inner;

            //__________________________________________________________________
            //
            // X
            //__________________________________________________________________
            {
                const domain<coord3d>::link &xlnk = L.links[0];
                const size_t                     ng  = L.depth*outer.width.y*outer.width.z;
                ghosts &G = (*this)[0];
                if(xlnk.prev)
                {
                    ghost &g = * (G.prev = new ghost(xlnk.prev->rank,ng));
                    for(coord1d x=outer.lower.x;x<inner.lower.x;++x)
                    {
                        for(coord1d y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            for(coord1d z=outer.lower.z;z<=outer.upper.z;++z)
                            {
                                const coord3d q(x,y,z);
                                const coord1d v = outer.offset_of(q);
                                //std::cerr << "xprev@" << q << " => " << v <<std::endl;
                                g.push_back(v);
                            }
                        }
                    }
                    assert(g.size()==ng);
                }

                if(xlnk.next)
                {
                    ghost &g = * (G.next = new ghost(xlnk.next->rank,ng));
                    for(coord1d x=inner.upper.x+1;x<=outer.upper.x;++x)
                    {
                        for(coord1d y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            for(coord1d z=outer.lower.z;z<=outer.upper.z;++z)
                            {
                                const coord3d q(x,y,z);
                                const coord1d v = outer.offset_of(q);
                                //std::cerr << "xnext@" << q << " => " << v <<std::endl;
                                g.push_back(v);
                            }
                        }
                    }
                    assert(g.size()==ng);
                }
            }

            //__________________________________________________________________
            //
            // Y
            //__________________________________________________________________
            {
                const domain<coord3d>::link &ylnk = L.links[1];
                const size_t                     ng  = L.depth*outer.width.x*outer.width.z;
                ghosts &G = (*this)[1];
                if(ylnk.prev)
                {
                    ghost &g = *( G.prev = new ghost(ylnk.prev->rank,ng));
                    for(coord1d x=outer.lower.x;x<=outer.upper.x;++x)
                    {
                        for(coord1d y=outer.lower.y;y<inner.lower.y;++y)
                        {
                            for(coord1d z=outer.lower.z;z<=outer.upper.z;++z)
                            {
                                const coord3d q(x,y,z);
                                const coord1d v = outer.offset_of(q);
                                //std::cerr << "yprev@" << q << " => " << v <<std::endl;
                                g.push_back(v);
                            }
                        }
                    }
                    assert(g.size()==ng);
                }

                if(ylnk.next)
                {
                    ghost &g = *( G.next = new ghost(ylnk.next->rank,ng));
                    for(coord1d x=outer.lower.x;x<=outer.upper.x;++x)
                    {
                        for(coord1d y=inner.upper.y+1;y<=outer.upper.y;++y)
                        {
                            for(coord1d z=outer.lower.z;z<=outer.upper.z;++z)
                            {
                                const coord3d q(x,y,z);
                                const coord1d v = outer.offset_of(q);
                                //std::cerr << "yprev@" << q << " => " << v <<std::endl;
                                g.push_back(v);
                            }
                        }
                    }
                    assert(g.size()==ng);
                }
                
            }


            //__________________________________________________________________
            //
            // Z
            //__________________________________________________________________
            {
                const domain<coord3d>::link &zlnk = L.links[2];
                const size_t                     ng  = L.depth*outer.width.x*outer.width.y;
                ghosts &G = (*this)[2];
                if(zlnk.prev)
                {
                    ghost &g = *( G.prev = new ghost(zlnk.prev->rank,ng));
                    for(coord1d x=outer.lower.x;x<=outer.upper.x;++x)
                    {
                        for(coord1d y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            for(coord1d z=outer.lower.z;z<inner.lower.z;++z)
                            {
                                const coord3d q(x,y,z);
                                const coord1d v = outer.offset_of(q);
                                //std::cerr << "zprev@" << q << " => " << v <<std::endl;
                                g.push_back(v);
                            }
                        }
                    }
                    assert(g.size()==ng);
                }

                if(zlnk.next)
                {
                    ghost &g = *( G.next = new ghost(zlnk.next->rank,ng));
                    for(coord1d x=outer.lower.x;x<=outer.upper.x;++x)
                    {
                        for(coord1d y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            for(coord1d z=inner.upper.z+1;z<=outer.upper.z;++z)
                            {
                                const coord3d q(x,y,z);
                                const coord1d v = outer.offset_of(q);
                                //std::cerr << "zprev@" << q << " => " << v <<std::endl;
                                g.push_back(v);
                            }
                        }
                    }
                    assert(g.size()==ng);
                }



            }

        }
        
        
        
    }
    
}
