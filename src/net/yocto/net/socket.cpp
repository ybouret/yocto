#include "yocto/net/socket.hpp"
#include <iostream>
#include "yocto/exception.hpp"


namespace yocto {

    namespace network {

        
        const sock_key_t & socket:: key() const throw()
        {
            return fdvalue;
        }


        socket:: ~socket() throw()
        {
            bsd::socket_close( socket_ );
        }

        void socket:: shutdown( const shutdown_mode sd  ) throw()
        {
            bsd::socket_shutdown( socket_ , sd );
        }

        void socket:: blocking( bool do_block ) throw()
        {
            bsd::socket_set_blocking( socket_, do_block );
        }

        const socket_address  &socket:: self() const throw()
        {
            return ipaddr_;
        }

    }

}

namespace yocto {

    namespace network {

        //-- TCP server
        socket:: socket( const socket_address &ip, const int max_pending, type2type<tcp_server> ) :
        ipaddr_( ip ),
        socket_( bsd::socket_listen(
                                    //-- <listen>
                                    bsd::socket_bind(
                                                     //-- <bind>
                                                     bsd::socket_set_default_options(
                                                                                     net::instance().open_tcp( ipaddr_.fmt.af, ipaddr_.fmt.version )
                                                                                     ),
                                                     ipaddr_
                                                     //-- </bind>
                                                     ),
                                    max_pending
                                    //-- </listen>
                                    )
                ),
        ioflag_( 0 ),
        sending_( false ),
        sending( sending_, false, "tcp_server sending"),
        fdvalue( static_cast<ptrdiff_t>(socket_) )
        {
        }

    }

}

namespace yocto {

    namespace network {

        //-- TCP client
        socket:: socket( const socket_address &ip, type2type<tcp_client> ) :
        ipaddr_( ip ),
        socket_(
                bsd::socket_connect(
                                    //-- <connect>
                                    bsd::socket_set_default_options(
                                                                    net::instance().open_tcp( ipaddr_.fmt.af, ipaddr_.fmt.version )
                                                                    ),
                                    ipaddr_
                                    //-- </connect>
                                    )
                ),
        ioflag_(0),
        sending_( false ),
        sending( sending_, true, "tcp_client sending"),
        fdvalue( static_cast<ptrdiff_t>(socket_) )
        {
        }


        socket:: socket( const socket_address_format &f, socket_t srv_sock, type2type<tcp_client> ) :
        ipaddr_( f, socket_address_none, 0 ),
        socket_(
                bsd::socket_set_default_options(
                                                bsd::socket_accept( srv_sock, ipaddr_ )
                                                )
                ),
        ioflag_( 0 ),
        sending_( false ),
        sending( sending_, true, "tcp_client sending" ),
        fdvalue( static_cast<ptrdiff_t>(socket_) )
        {
        }


    }

}

namespace yocto {

    namespace network {

        //! UDP client
        socket:: socket( const socket_address &ip, type2type<udp_client> ) :
        ipaddr_( ip ),
        socket_(  bsd::socket_set_default_options(
                                                  net::instance().open_udp( ipaddr_.fmt.af, ipaddr_.fmt.version )
                                                  )
                ),
        ioflag_( 0  ),
        sending_( false ),
        sending( sending_, true, "udp_client sending"),
        fdvalue( static_cast<ptrdiff_t>(socket_) )
        {
        }


    }

}

namespace yocto {

    namespace network {

        // UDP server
        socket:: socket( const socket_address &ip, type2type<udp_server> ) :
        ipaddr_( ip ),
        socket_(  bsd::socket_bind(
                                   //-- <bind>
                                   bsd::socket_set_default_options(
                                                                   net::instance().open_udp( ipaddr_.fmt.af, ipaddr_.fmt.version )
                                                                   ),
                                   ipaddr_
                                   //-- </bind>
                                   )
                ),
        ioflag_( 0 ),
        sending_( false ),
        sending( sending_, true, "udp server sending"),
        fdvalue( static_cast<ptrdiff_t>(socket_) )
        {
        }


    }

}

namespace yocto {

    namespace network {
        
        size_t socket:: sndbuf() const throw()
        {
            sol_socket_parameter p = { so_sndbuf, {0} };
            try
            {
                bsd::get_socket_option( socket_, p );
                return p.sndbuf_value;
            }
            catch(const exception &e)
            {
                std::cerr << e.what() << ": " << e.when() << std::endl;
                return 0;
            }
        }
        
        size_t socket:: rcvbuf() const throw()
        {
            sol_socket_parameter p = { so_rcvbuf, {0} };
            try
            {
                bsd::get_socket_option( socket_, p );
                return p.rcvbuf_value;
            }
            catch(const exception &e)
            {
                std::cerr << e.what() << ": " << e.when() << std::endl;
                return 0;
            }
        }
        
        
    }
    
}
