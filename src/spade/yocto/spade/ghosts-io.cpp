#include "yocto/spade/ghosts-io.hpp"

namespace yocto
{
    namespace spade
    {
        ghosts_io:: ~ghosts_io() throw()
        {

        }



    }
}

namespace yocto
{
    namespace spade
    {


        ghosts_io:: ghosts_io( const field_layouts1D &F ) : IO(2,as_capacity)
        {
            const ghosts1D &gs    = F.ghosts;
            const layout1D &outer = F.outer;
            const layout1D &inner = F.inner;

            //__________________________________________________________________
            //
            // lower
            //__________________________________________________________________
            {
                const ghost1D    &g = gs.lower;
                coord1D           o = outer.lower;
                coord1D           i = inner.lower;
                exchange::pointer xch( new exchange(g.peer,g.size) );

                std::cerr << "lower#ghosts=" << g.size << std::endl;
                for(coord1D u=g.size;u>0;--u,++o,++i)
                {
                    xch->recv.push_back(outer.offset_of(o));
                    xch->send.push_back(outer.offset_of(i));
                }

                co_qsort(xch->recv,xch->send);
                std::cerr << "lower recv: " << xch->recv << std::endl;
                std::cerr << "lower send: " << xch->send << std::endl;
                IO.push_back(xch);
            }

            //__________________________________________________________________
            //
            // upper
            //__________________________________________________________________
            {
                const ghost1D    &g = gs.upper;
                coord1D           o = outer.upper;
                coord1D           i = inner.upper;
                exchange::pointer xch( new exchange(g.peer,g.size) );

                std::cerr << "upper#ghosts=" << g.size << std::endl;
                for(coord1D u=g.size;u>0;--u,--o,--i)
                {
                    xch->recv.push_back(outer.offset_of(o));
                    xch->send.push_back(outer.offset_of(i));
                }
                
                co_qsort(xch->recv,xch->send);
                std::cerr << "upper recv: " << xch->recv << std::endl;
                std::cerr << "upper send: " << xch->send << std::endl;
                IO.push_back(xch);
            }
            
            
        }
        
    }
    
}

namespace yocto
{
    namespace spade
    {

        ghosts_io:: ghosts_io( const field_layouts2D &F ) :
        IO(4,as_capacity)
        {

            const ghosts2D &gs    = F.ghosts;
            const layout2D &outer = F.outer;
            const layout2D &inner = F.inner;

            //__________________________________________________________________
            //
            // lower
            //__________________________________________________________________
            {

                //______________________________________________________________
                //
                // lower.x
                //______________________________________________________________
                {
                    const ghost2D    &g  = gs.lower;
                    const coord1D     ny = outer.width.y;
                    coord1D           o  = outer.lower.x;
                    coord1D           i  = inner.lower.x;
                    const size_t      ng = g.size.x*ny;
                    exchange::pointer xch( new exchange(g.peer.x,ng) );

                    std::cerr << "x_lower#ghosts=" << ng << " @peer=" << xch->peer << std::endl;
                    for(coord1D u=g.size.x;u>0;--u,++o,++i)
                    {
                        for(coord1D y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            const coord2D O(o,y);
                            xch->recv.push_back(outer.offset_of(O));
                            const coord2D I(i,y);
                            xch->send.push_back(outer.offset_of(I));
                        }
                    }
                    IO.push_back(xch);
                    co_qsort(xch->recv,xch->send);
                    std::cerr << "x_lower recv=" << xch->recv << std::endl;
                    std::cerr << "x_lower send=" << xch->send << std::endl;

                }


                //______________________________________________________________
                //
                // upper.x
                //______________________________________________________________
                {
                    const ghost2D    &g  = gs.upper;
                    const coord1D     ny = outer.width.y;
                    coord1D           o  = outer.upper.x;
                    coord1D           i  = inner.upper.x;
                    const size_t      ng = g.size.x*ny;
                    exchange::pointer xch( new exchange(g.peer.x,ng) );

                    std::cerr << "x_upper#ghosts=" << ng << " @peer=" << xch->peer << std::endl;
                    for(coord1D u=g.size.x;u>0;--u,--o,--i)
                    {
                        for(coord1D y=outer.lower.y;y<=outer.upper.y;++y)
                        {
                            const coord2D O(o,y);
                            xch->recv.push_back(outer.offset_of(O));
                            const coord2D I(i,y);
                            xch->send.push_back(outer.offset_of(I));
                        }
                    }
                    IO.push_back(xch);
                    co_qsort(xch->recv,xch->send);
                    std::cerr << "x_upper recv=" << xch->recv << std::endl;
                    std::cerr << "x_upper send=" << xch->send << std::endl;
                }


                //______________________________________________________________
                //
                // lower.y
                //______________________________________________________________
                {
                    const ghost2D    &g  = gs.lower;
                    const coord1D     nx = outer.width.x;
                    coord1D           o  = outer.lower.y;
                    coord1D           i  = inner.lower.y;
                    const size_t      ng = g.size.y*nx;
                    exchange::pointer xch( new exchange(g.peer.y,ng) );

                    std::cerr << "y_lower#ghosts=" << ng << " @peer=" << xch->peer << std::endl;
                    for(coord1D u=g.size.y;u>0;--u,++o,++i)
                    {
                        for(coord1D x=outer.lower.x;x<=outer.upper.x;++x)
                        {
                            const coord2D O(x,o);
                            xch->recv.push_back(outer.offset_of(O));
                            const coord2D I(x,i);
                            xch->send.push_back(outer.offset_of(I));
                        }
                    }
                    IO.push_back(xch);
                    co_qsort(xch->recv,xch->send);
                    std::cerr << "y_lower recv=" << xch->recv << std::endl;
                    std::cerr << "y_lower send=" << xch->send << std::endl;
                }

                //______________________________________________________________
                //
                // upper.y
                //______________________________________________________________
                {
                    const ghost2D    &g  = gs.upper;
                    const coord1D     nx = outer.width.x;
                    coord1D           o  = outer.upper.y;
                    coord1D           i  = inner.upper.y;
                    const size_t      ng = g.size.y*nx;
                    exchange::pointer xch( new exchange(g.peer.y,ng) );

                    std::cerr << "x_upper#ghosts=" << ng << " @peer=" << xch->peer << std::endl;
                    for(coord1D u=g.size.y;u>0;--u,--o,--i)
                    {
                        for(coord1D x=outer.lower.x;x<=outer.upper.x;++x)
                        {
                            const coord2D O(x,o);
                            xch->recv.push_back(outer.offset_of(O));
                            const coord2D I(x,i);
                            xch->send.push_back(outer.offset_of(I));
                        }
                    }
                    IO.push_back(xch);
                    co_qsort(xch->recv,xch->send);
                    std::cerr << "y_upper recv=" << xch->recv << std::endl;
                    std::cerr << "y_upper send=" << xch->send << std::endl;
                }

                
            }
        }

    }
}




