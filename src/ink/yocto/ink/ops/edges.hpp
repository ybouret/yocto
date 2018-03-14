#ifndef YOCTO_INK_OPS_EDGES_INCLUDED
#define YOCTO_INK_OPS_EDGES_INCLUDED 1

#include "yocto/ink/ops/blob.hpp"
#include "yocto/ink/ops/blur.hpp"
#include "yocto/ink/ops/gradients.hpp"

namespace yocto
{
    namespace Ink
    {

        class Edges : public Pixmap<uint8_t>
        {
        public:
            static const uint8_t WEAK   = 127;
            static const uint8_t STRONG = 255;

            virtual ~Edges() throw();
            explicit Edges(const unit_t W, const unit_t H);

            Pixmap<float>   intensity;
            Gradients       gradient;
            Pixmap<uint8_t> maxima;    //!< map of graded maxima
            Blob            tags;      //!< to find edges as particles
            Pixmap<float>   temporary; //!< for blurring

            void  loadIntensity(const Pixmap<float> &source,
                                Blur                *blur,
                                Engine              &engine);

            //! compute not-normalized gradient
            void computeGradient(const Stencil       &dx,
                                 const Stencil       &dy,
                                 Engine              &engine);

            //! keep gradient only if local maxima in the gradient direction
            /**
             The maximum M is stored as 8bits indicator from G/Gmax,
             and the statistical strong and weak levels are computed
             */
            void keepLocalMaxima(Engine &engine);

            //! grade maxima into this, according to weak and strong scores
            void  statisticalLevels(Engine &engine);

            //! build particles
            void  build(Particles &particles, Particles *weaks=NULL);

            //! all in one call
            void detect(Particles           &particles,
                        Particles           *weaks,
                        const Pixmap<float> &source,
                        const Stencil       &dx,
                        const Stencil       &dy,
                        Blur                *blur,
                        Engine              &engine);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Edges);
            void  maximaThread( const Domain &dom, threading::context & ) throw();
            void  statisticalThread(const Domain &dom, threading::context & ) throw();
            
        public:
            size_t strong;
            size_t weak;
        };

        
        
    }
}

#endif

