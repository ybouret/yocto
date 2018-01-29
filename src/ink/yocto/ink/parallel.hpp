#ifndef YOCTO_INK_PARALLEL_INCLUDED
#define YOCTO_INK_PARALLEL_INCLUDED 1

#include "yocto/ink/bitmap.hpp"
#include "yocto/math/point2d.hpp"
#include "yocto/core/list.hpp"
#include "yocto/threading/scheme/server.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef point2d<unit_t> coord;

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


        class Partition : public Area
        {
        public:
            typedef core::list_of_cpp<Partition> List;

            const coord   sizes;     //!< sizes in different dimension
            const size_t  cores;     //!< total number of cores, MPI style
            const size_t  score;     //!< maximum size of sub rectangles
            Partition    *next;
            Partition    *prev;
            
            explicit Partition(const coord user_sizes, const Area &rect) throw();
            virtual ~Partition() throw();
            
            coord      getRanks(const size_t rank) const throw();
            size_t     getRank(const coord  ranks) const throw();
            Area       operator()(const size_t rank, coord *pRanks) const throw();

            //! (re)compute score, build domains if not null
            void          compute( Domain::List *domains ) const;
            static void   Build(List            &parts,
                                const Area      &full,
                                const size_t     max_cpus );

            static coord  Optimal(const Area &full, const size_t max_cpus);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Partition);
        };

        typedef threading::server      ThreadServer;
        typedef ThreadServer::pointer  SharedServer;

        class Domains : public Domain::List
        {
        public:
            mutable SharedServer srv;

            explicit Domains(const Bitmap       &bmp,
                             const SharedServer &user_srv);
            explicit Domains(const Area         &area,
                             const SharedServer &user_src);
            
            virtual ~Domains() throw();

            //! enqueue #jobs=size, METHOD(const Rectangle &, threading::context &ctx)
            template <typename OBJECT_POINTER,typename METHOD_POINTER> inline
            void enqueue(OBJECT_POINTER      host,
                         METHOD_POINTER      method ) const
            {
                for(const Domain *dom = head;dom;dom=dom->next)
                {
                    dom->jid = srv->enqueue_shared(*dom,host,method);
                }
                srv->flush();
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Domains);
        };
        

        
    }
}

#endif
