#ifndef YOCTO_INK_PARALLEL_INCLUDED
#define YOCTO_INK_PARALLEL_INCLUDED 1

#include "yocto/ink/bitmap.hpp"
#include "yocto/core/list.hpp"
#include "yocto/threading/scheme/server.hpp"

namespace yocto
{
    namespace Ink
    {

        class Domain : public Area
        {
        public:
            typedef core::list_of_cpp<Domain> List;

            const coord  ranks;
            const size_t items;
            const size_t rank;
            Domain      *next;
            Domain      *prev;
            mutable  threading::job_id jid;
            explicit Domain(const Area      &user_rect,
                            const coord     &user_ranks,
                            const size_t     user_rank);

            virtual ~Domain() throw();


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Domain);
        };



        typedef threading::server      ThreadServer;
        typedef ThreadServer::pointer  SharedServer;

        class Domains : public Domain::List
        {
        public:
            mutable SharedServer srv;
            const   coord        sizes; //!< keep track of paritions

            explicit Domains(const Bitmap       &bmp,
                             const SharedServer &user_srv);
            explicit Domains(const Area         &area,
                             const SharedServer &user_src);
            
            virtual ~Domains() throw();

            //! enqueue #jobs=size, METHOD(const Rectangle &, threading::context &ctx)
            template <typename OBJECT_POINTER,typename METHOD_POINTER> inline
            void submit(OBJECT_POINTER      host,
                        METHOD_POINTER      method ) const
            {
                for(const Domain *dom = head;dom;dom=dom->next)
                {
                    dom->jid = srv->enqueue_shared(*dom,host,method);
                }
                srv->flush();
            }

            static coord GetPartitionFor(const Area &area, const SharedServer &srv) throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Domains);
            void setup(const Area &full);
        };

        
    }
}

#endif
