
#include "yocto/ink/ops/edges-detector.hpp"

namespace yocto
{
    namespace Ink
    {
        EdgesDetector:: ~EdgesDetector() throw()
        {
        }
        
        EdgesDetector:: EdgesDetector(const unit_t W, const unit_t H) :
        Edges(W,H),
        particles()
        {
        }
    }
    
}

