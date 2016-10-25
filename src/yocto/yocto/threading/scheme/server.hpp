#ifndef YOCTO_THREADIND_SCHEME_SERVER_INCLUDED
#define YOCTO_THREADIND_SCHEME_SERVER_INCLUDED 1

#include "yocto/threading/scheme/executor.hpp"

namespace yocto
{
    namespace threading
    {
        typedef uint32_t job_id;

        //! server interface
        class server
        {
        public:
            executor &cpu;

            virtual ~server() throw();

            virtual job_id enqueue( kernel &k ) = 0;


        protected:
            explicit server(executor &xxx) throw();
            job_id   juuid;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(server);
        };


        class seq_server : public seq_executor, public server
        {
        public:
            virtual ~seq_server() throw();
            explicit seq_server();

            virtual job_id enqueue( kernel &k );

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(seq_server);
        };


    }
}

#endif
