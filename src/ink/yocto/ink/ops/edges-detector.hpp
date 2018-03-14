
#ifndef YOCTO_EDGES_DETECTOR_INCLUDED
#define YOCTO_EDGES_DETECTOR_INCLUDED 1

#include "yocto/ink/ops/edges.hpp"
#include "yocto/ink/ops/sobel.hpp"

namespace yocto
{
    namespace Ink
    {
        
        class EdgesDetector : public Edges
        {
        public:
            Particles firm; //!< strong edges
            Particles thin; //!< weak edges
            
            
        protected:
            inline explicit EdgesDetector(const unit_t W, const unit_t H) :
            Edges(W,H),
            firm(),
            thin()
            {
            }
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(EdgesDetector);
        };
        
        template <typename DX, typename DY>
        class EdgesDetectorWith : public EdgesDetector
        {
        public:
            const DX  dx;
            const DY  dy;
           
            
            inline explicit EdgesDetectorWith(const unit_t W, const unit_t H) :
            EdgesDetector(W,H),
            dx(),
            dy()
            {
            }
            
            inline virtual ~EdgesDetectorWith() throw()
            {
            }
            
            inline void detect(const Pixmap<float> &source,
                               Blur                 *blur,
                               Engine              &engine,
                               const bool            keep)
            {
                this->Edges::detect(firm,NULL,source,dx,dy,blur,engine);
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(EdgesDetectorWith);
        };
        
    }
    
}

#endif

