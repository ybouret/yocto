#ifndef YOCTO_THREADIND_SCHEME_SERVER_INCLUDED
#define YOCTO_THREADIND_SCHEME_SERVER_INCLUDED 1

#include "yocto/threading/scheme/executor.hpp"

namespace yocto
{
    namespace threading
    {
        typedef uint32_t job_id;

        //______________________________________________________________________
        //
        //! server interface
        //______________________________________________________________________
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

        //______________________________________________________________________
        //
        //! simple sequential server....
        //______________________________________________________________________
        class seq_server : public seq_executor, public server
        {
        public:
            virtual ~seq_server() throw();
            explicit seq_server();

            virtual job_id enqueue( kernel &k );

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(seq_server);
        };


        //______________________________________________________________________
        //
        //! parallel server
        //______________________________________________________________________
        class par_server : public par_executor, public server
        {
        public:
            virtual ~par_server() throw();
            explicit par_server(const bool setVerbose=false);



        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(par_server);
            static void control_loop( void *args ) throw();
            static void workers_loop( void *args ) throw();

            class task : public object
            {
            public:
                task         *next;
                task         *prev;
                const  job_id uuid;
                kernel        proc;

                task(const job_id j, const kernel &k);
                ~task() throw();

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(task);
            };

            typedef core::pool_of<task>     task_pool; //!< dead tasks
            typedef core::pool_of_cpp<task> task_list; //!< alive tasks
            task_list pending; //!< tasks to be done
            task_list current; //!< tasks being processed
            task_pool storage; //!< cache

        };

    }
}

#endif
