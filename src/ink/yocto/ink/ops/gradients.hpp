#ifndef YOCTO_INK_GRADIENTS_INCLUDED
#define YOCTO_INK_GRADIENTS_INCLUDED 1

#include "yocto/ink/ops/stencil.hpp"

namespace yocto
{
    namespace Ink
    {
        class Gradients : public Area
        {
        public:
            typedef Pixmap<float> pixmap;
            typedef pixmap::Row   row;
            static const size_t   BytesPerDomain = sizeof(float);
            
        protected:
            Pixmaps<float> fields;
            
        public:
            pixmap &gx; //!< x gradient
            pixmap &gy; //!< y gradient
            pixmap &gn; //!< norm(gx,gy)
            pixmap &ga; //!< angle
            float   gn_max;
            
            explicit Gradients(const Bitmap &bmp);
            virtual ~Gradients() throw();
          
            void compute(const pixmap  &source,
                         const Stencil &dx,
                         const Stencil &dy,
                         Engine        &engine,
                         const bool     normalize);
            
        private:
            const pixmap  *src;
            const Stencil *sdx;
            const Stencil *sdy;
            void  computeThread( const Domain &dom, threading::context &) throw();
            void  normalizeThread( const Domain &dom, threading::context &) throw();

        };
        
    }
}

#endif

