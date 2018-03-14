#ifndef YOCTO_INK_OPS_EDGES_INCLUDED
#define YOCTO_INK_OPS_EDGES_INCLUDED 1

#include "yocto/ink/ops/blob.hpp"
#include "yocto/ink/ops/blur.hpp"
#include "yocto/ink/ops/histogram.hpp"
#include "yocto/ink/ops/gradients.hpp"

namespace yocto
{
    namespace Ink
    {

        class Edges : public Pixmap<uint8_t>
        {
        public:
            virtual ~Edges() throw();
            explicit Edges(const unit_t W, const unit_t H);

            Gradients       gradient;
            Pixmap<uint8_t> maxima; //!< map of graded maxima

            //! compute not-normalized gradient
            void computeGradient(const Pixmap<float> &intensity,
                                 const Stencil       &dx,
                                 const Stencil       &dy,
                                 Engine              &engine);

            //! keep gradient only if local maxima in the gradient direction
            /**
             The maximum M is stored as 8bits indicator from G/Gmax,
             and the statistical strong and weak level are computed
             */
            void keepLocalMaxima(Engine &engine);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Edges);
            void  maximaThread( const Domain &dom, threading::context & ) throw();

        public:
            size_t strong;
            size_t weak;
        };

#if 0
        class Edges : public Pixmap<uint8_t>
        {
        public:
            static const uint8_t WEAK   = 127;
            static const uint8_t STRONG = 255;
            
            explicit Edges(const unit_t W, const unit_t H);
            virtual ~Edges() throw();

            Pixmap<float>   intensity; //!< source intensity
            Pixmap<float>   gradient;  //!< gradient
            Pixmap<float>   direction; //!< gradient direction
            Pixmap<uint8_t> maxima;    //!< local maximum only
            Blob            tags;      //!< edges tags
            
            //! compute G from S, and angle
            /**
             The maximal gradient Gmax is computed as well
             */
            void computeGradient(Engine &engine);
            
            //! keep gradient only if local maxima in the gradient direction
            /**
             The maximum M is stored as 8bits indicator from G/Gmax,
             and the statistical strong and weak level are computed
             */
            void keepLocalMaxima(Engine &engine);

            //! grade maxima into edges according to weak and strong
            void grade( Engine &engine );

            //!
            void connect(Particles &particles, Engine &engine);
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Edges);
            void  gradientThread( const Domain &dom, threading::context & ) throw();
            void  maximaThread( const Domain &dom, threading::context & ) throw();
            void  gradeThread( const Domain &dom, threading::context & ) throw();
            
        public:
            float     Gmax;
            size_t    strong;
            size_t    weak;
        };
#endif
        
    }
}

#endif

