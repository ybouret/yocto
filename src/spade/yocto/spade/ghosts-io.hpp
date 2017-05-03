#ifndef YOCTO_SPADE_GHOSTS_IO_INCLUDED
#define YOCTO_SPADE_GHOSTS_IO_INCLUDED 1

#include "yocto/spade/field.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/counted.hpp"
#include "yocto/ptr/arc.hpp"
#include "yocto/sort/quick.hpp"

namespace yocto
{
    namespace spade
    {



        class ghosts_io
        {
        public:
            typedef vector<coord1D> indices_type;

            class indices : public counted, public indices_type
            {
            public:
                typedef arc_ptr<indices> pointer;
                const int peer;

                explicit indices(const int    whom,
                                 const size_t n) :
                indices_type(n,as_capacity),
                peer(whom)
                {
                }

                virtual ~indices() throw() {}

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(indices);
            };

            virtual ~ghosts_io() throw();

            ghosts_io( field_layouts1D &F )
            {
                const ghosts1D &gs    = F.ghosts;
                const layout1D &outer = F.outer;
                const layout1D &inner = F.inner;

                // lower
                {
                    const ghost1D  &g = gs.lower;
                    coord1D         o = outer.lower;
                    coord1D         i = inner.lower;
                    indices::pointer odx( new indices(g.peer,g.size) );
                    indices::pointer idx( new indices(g.peer,g.size) );

                    std::cerr << "#ghosts=" << g.size << std::endl;
                    for(coord1D u=g.size;u>0;--u,++o,++i)
                    {
                        odx->push_back( outer.offset_of(o) );
                        idx->push_back( outer.offset_of(i) );
                    }

                    co_qsort(*odx,*idx);
                    std::cerr << "lower odx: " << (indices_type&)*odx << std::endl;
                    std::cerr << "lower idx: " << (indices_type&)*idx << std::endl;
                }

                // upper
                {
                    const ghost1D  &g = gs.upper;
                    coord1D         o = outer.upper;
                    coord1D         i = inner.upper;
                    indices::pointer odx( new indices(g.peer,g.size) );
                    indices::pointer idx( new indices(g.peer,g.size) );

                    std::cerr << "#ghosts=" << g.size << std::endl;
                    for(coord1D u=g.size;u>0;--u,--o,--i)
                    {
                        odx->push_back( outer.offset_of(o) );
                        idx->push_back( outer.offset_of(i) );
                    }

                    co_qsort(*odx,*idx);

                    std::cerr << "upper odx: " << (indices_type&)*odx << std::endl;
                    std::cerr << "upper idx: " << (indices_type&)*idx << std::endl;

                }


            }

            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_io);

        };

    }
}

#endif
