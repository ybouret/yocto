#ifndef YOCTO_INK_OPS_EDGES_INCLUDED
#define YOCTO_INK_OPS_EDGES_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ink/pixmaps.hpp"
#include "yocto/ink/ops/blur.hpp"
#include "yocto/ink/ops/histogram.hpp"

namespace yocto
{
    namespace Ink
    {

        class Edges : public Pixmap<uint8_t>
        {
        public:
            explicit Edges(const unit_t W, const unit_t H);
            virtual ~Edges() throw();

            Pixmap<float>   S; //!< source intensity
            Pixmap<float>   G; //!< gradient
            Pixmap<float>   A; //!< angle
            Pixmap<uint8_t> M; //!< local maximum only
            void computeGradient(Engine &engine);
            void keepLocalMaxima(Engine &engine);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Edges);
            void  gradThread( const Domain &dom, threading::context & ) throw();
            void  keepThread( const Domain &dom, threading::context & ) throw();

        public:
            float     Gmax;
            Histogram H;
        };
    }
}

#endif

