#ifndef YOCTO_INK_PARALLEL_INCLUDED
#define YOCTO_INK_PARALLEL_INCLUDED 1

#include "yocto/ink/bitmap.hpp"
#include "yocto/core/list.hpp"
#include "yocto/threading/scheme/server.hpp"
#include "yocto/container/cslot.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef threading::job_id JobID;
        
        //! a Domain is compute for computation on a sub are
        class Domain : public Area
        {
        public:
            typedef core::list_of_cpp<Domain> List;

            const coord   ranks; //!< 2D ranks in the Domains sizes
            const size_t  items; //!< number of local items
            const size_t  rank;  //!< global rank in Domain::List
            Domain       *next;  //!< for Domain::List
            Domain       *prev;  //!< for Domain::List
            mutable JobID jid;   //!< to store job_id during parallel code
            mutable cslot cache; //!< for thread specific memory

            //! constructor
            explicit Domain(const Area      &user_rect,
                            const coord     &user_ranks,
                            const size_t     user_rank) throw();

            virtual ~Domain() throw();


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Domain);
        };
        

        class Engine : public Area
        {
        public:
            typedef threading::server::pointer SharedServer;
            const coord  sizes; //!< how this engine is partitioned on the area
            SharedServer queue; //!< threads to enqueue jobs

            explicit Engine(const Area &area, const SharedServer &sharedServer);
            virtual ~Engine() throw();

            //! enqueue #jobs=size, METHOD(const Domain &, threading::context &ctx)
            template <typename OBJECT_POINTER,typename METHOD_POINTER> inline
            void submit(OBJECT_POINTER      host,
                        METHOD_POINTER      method)
            {
                threading::server &Q  =  *queue;
                for(const Domain *dom = domains.head;dom;dom=dom->next)
                {
                    dom->jid = Q.enqueue_shared(*dom,host,method);
                }
                Q.flush();
            }

            const Domain *head() const throw();
            size_t        size() const throw();

            void prepare(const size_t bytes_per_domain);

        private:
            Domain::List domains;
            YOCTO_DISABLE_COPY_AND_ASSIGN(Engine);

        };

        
    }
}

#endif
