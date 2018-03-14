
#ifndef YOCTO_EDGES_DETECTOR_INCLUDED
#define YOCTO_EDGES_DETECTOR_INCLUDED 1

#include "yocto/ink/ops/edges.hpp"
#include "yocto/ink/ops/sobel.hpp"
#include "yocto/ink/ops/scharr.hpp"

namespace yocto
{
    namespace Ink
    {
        
        class EdgesDetector : public Edges
        {
        public:
            Particles particles; //!< strong edges coordinates
            
            virtual void find(const Pixmap<float> &source,
                              Particles           *weaks,
                              Blur                *blur,
                              Engine              &engine) = 0;
            
            virtual ~EdgesDetector() throw();
            
        protected:
            explicit EdgesDetector(const unit_t W, const unit_t H);
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(EdgesDetector);
        };
        
        template <class DX = Scharr5X, class DY = Scharr5Y>
        class EdgesRadar : public EdgesDetector
        {
        public:
            const DX dx;
            const DY dy;
            
            inline explicit EdgesRadar(const unit_t W, const unit_t H) :
            EdgesDetector(W,H),dx(),dy() {}
            
            inline virtual ~EdgesRadar() throw() {}
            
            virtual void find(const Pixmap<float> &source,
                              Particles           *weaks,
                              Blur                *blur,
                              Engine             &engine)
            {
                this->detect(particles, weaks, source, dx, dy, blur, engine);
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(EdgesRadar);
        };
        
    }
    
}

#endif

