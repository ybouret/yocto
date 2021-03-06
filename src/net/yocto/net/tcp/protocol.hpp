#ifndef YOCTO_NET_TCP_PROTOCOL_INCLUDED
#define YOCTO_NET_TCP_PROTOCOL_INCLUDED 1

#include "yocto/net/tcp/connexion.hpp"
#include "yocto/net/socket/set.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/ptr/intr.hpp"
#include "yocto/sequence/list.hpp"

namespace yocto
{
    namespace network
    {

        //! common architecture for tcp protocol
        class tcp_protocol
        {
        public:
            //__________________________________________________________________
            //
            // types definition
            //__________________________________________________________________
            typedef intr_ptr<sock_key_t,connexion> conn_ptr;
            typedef set<sock_key_t,conn_ptr>       conn_set;
            typedef conn_set::iterator             conn_iter;
            typedef list<sock_key_t>               keys_list;

            virtual ~tcp_protocol() throw();


            //__________________________________________________________________
            //
            // common API
            //__________________________________________________________________
            virtual void onQuit(connexion &) throw();
            virtual void onRecv(connexion &);
            virtual void onInit(connexion &);
            virtual void onSent(connexion &);
            virtual void onIdle();

            void    link_to(const socket_address &ipaddr);
            
            //! set the running flag to false
            void    terminate() throw();

            io_cache   cache;

        protected:
            explicit tcp_protocol(size_t block_size);
            
            socket_set sockset;
            conn_set   conn_db;
            keys_list  dropped;
            bool       running;

            void disconnect();         //!< disconnect dropped connections
            bool initialize() throw(); //!< prepare each connection
            void check_recv();         //!< check what is received
            void check_send();         //!< check what is to be sent

            void enqueue( connexion *c ); //!< WARNING: close if !running


        public:
            double     every;

        };


    }

}

#endif
