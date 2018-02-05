#ifndef YOCTO_INK_PARALLEL_INCLUDED
#define YOCTO_INK_PARALLEL_INCLUDED 1

#include "yocto/ink/bitmap.hpp"
#include "yocto/core/list.hpp"
#include "yocto/threading/scheme/server.hpp"

namespace yocto
{
    namespace Ink
    {
        
        //! a Domain is compute for computation on a sub are
        class Domain : public Area
        {
        public:
            typedef core::list_of_cpp<Domain> List;

            const coord  ranks; //!< 2D ranks in the Domains sizes
            const size_t items; //!< number of local items
            const size_t rank;  //!< global rank in Domain::List
            Domain      *next;  //!< for Domain::List
            Domain      *prev;  //!< for Domain::List

            mutable  threading::job_id jid; //!< to store job_id during parallel code

            //! constructor
            explicit Domain(const Area      &user_rect,
                            const coord     &user_ranks,
                            const size_t     user_rank) throw();

            virtual ~Domain() throw();


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Domain);
        };



        typedef threading::server      ThreadServer; //!< alias for Ink usage
        typedef ThreadServer::pointer  SharedServer; //!< alias for Ink usage

        class Domains : public Domain::List
        {
        public:
            mutable SharedServer engine;
            const   coord        sizes; //!< keep track of paritions


            explicit Domains(const Area         &area,
                             const SharedServer &sharedServer);
            
            explicit Domains(const Area         &area,
                             ThreadServer       *threadServer);

            virtual ~Domains() throw();

            //! enqueue #jobs=size, METHOD(const Rectangle &, threading::context &ctx)
            template <typename OBJECT_POINTER,typename METHOD_POINTER> inline
            void submit(OBJECT_POINTER      host,
                        METHOD_POINTER      method ) const
            {
                for(const Domain *dom = head;dom;dom=dom->next)
                {
                    dom->jid = engine->enqueue_shared(*dom,host,method);
                }
                engine->flush();
            }

            static coord GetPartitionFor(const Area &area, const SharedServer &srv) throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Domains);
            void setup(const Area &full);
        };

        
        
    }
}

#endif
