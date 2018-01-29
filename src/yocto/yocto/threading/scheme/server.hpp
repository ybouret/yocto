#ifndef YOCTO_THREADIND_SCHEME_SERVER_INCLUDED
#define YOCTO_THREADIND_SCHEME_SERVER_INCLUDED 1

#include "yocto/threading/scheme/executor.hpp"
#include "yocto/threading/condition.hpp"
#include "yocto/sequence/array.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/arc.hpp"

namespace yocto
{
    namespace threading
    {
        typedef uint32_t job_id;

        //______________________________________________________________________
        //
        //! server interface
        //______________________________________________________________________
        class server : public counted_object
        {
        public:
            typedef arc_ptr<server> pointer;
            executor &cpu;

            virtual ~server() throw();

            virtual job_id enqueue( kernel &k ) = 0;
            virtual void   enqueue_all( array<kernel> &batch ) = 0;
            virtual void   flush() throw() = 0;

            //__________________________________________________________________
            //
            //! wrapper for single instruction on shared data
            /**
             METHOD_POINTER(T &, context &)
             */
            //__________________________________________________________________
            template <
            typename OBJECT_POINTER,
            typename METHOD_POINTER,
            typename T
            >
            class jshared
            {
            public:

                inline jshared(OBJECT_POINTER h, METHOD_POINTER c, T &d ) throw() :
                host(h), call(c), data(d) {}

                inline jshared(const jshared &other) throw() :
                host(other.host), call(other.call), data(other.data) {}

                inline ~jshared() throw() {}

                inline void operator()( context &ctx )
                {
                    ((*host).*call)(data,ctx);
                }

            private:
                OBJECT_POINTER host;
                METHOD_POINTER call;
                T             &data;
                YOCTO_DISABLE_ASSIGN(jshared);
            };


            template <typename OBJECT_POINTER,typename METHOD_POINTER,typename T> inline
            job_id enqueue_shared(T &data, OBJECT_POINTER host, METHOD_POINTER method)
            {
                const jshared<OBJECT_POINTER,METHOD_POINTER,T> js(host,method,data);
                kernel J(js);
                return enqueue(J);
            }

            //__________________________________________________________________
            //
            //! wrapper for single instruction and keep a copy of data
            /**
             METHOD_POINTER(T &, context &)
             */
            //__________________________________________________________________
            template <
            typename OBJECT_POINTER,
            typename METHOD_POINTER,
            typename T
            >
            class jcopy
            {
            public:

                inline jcopy(OBJECT_POINTER h, METHOD_POINTER c, const T &d ) throw() :
                host(h), call(c), data(d) {}

                inline jcopy(const jcopy &other) throw() :
                host(other.host), call(other.call), data(other.data) {}

                inline ~jcopy() throw() {}

                inline void operator()( context &ctx )
                {
                    ((*host).*call)(data,ctx);
                }

            private:
                OBJECT_POINTER host;
                METHOD_POINTER call;
                T              data;
                YOCTO_DISABLE_ASSIGN(jcopy);
            };


            template <typename OBJECT_POINTER,typename METHOD_POINTER,typename T> inline
            job_id enqueue_copy(const T &data, OBJECT_POINTER host, METHOD_POINTER method)
            {
                const jcopy<OBJECT_POINTER,METHOD_POINTER,T> jc(host,method,data);
                kernel J(jc);
                return enqueue(J);
            }



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
            virtual void   enqueue_all( array<kernel> &batch );
            virtual void   flush() throw();

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

            virtual job_id enqueue( kernel &k );
            virtual void   enqueue_all( array<kernel> &batch );
            virtual void   flush() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(par_server);
            static void start(void *args) throw();
            void   loop( context &) throw();

            void init();
            void quit() throw();

            class task
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

            typedef core::pool_of<task> task_pool; //!< dead tasks
            typedef core::list_of<task> task_list; //!< alive tasks

            task_list    pending;    //!< tasks to be done
            task_list    current;    //!< tasks being processed
            task_pool    storage;    //!< cache
            size_t       ready;      //!< for (first) synchronization
            condition    activity;   //!< workers are waiting for something to do
            condition    flushing;   //!< main thread is waiting for all jobs to complete
            
            task *create_task(const kernel &k);
        };
        
    }
}

#endif
