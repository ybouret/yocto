#include "yocto/spade/domain.hpp"

namespace yocto
{
    namespace spade
    {
        const char _Domain::Name[] = "spade.Domain";
        
        _Domain:: _Domain(const int r,const int s) throw() :
        size( (s<=0) ? 1 : s),
        rank( clamp<int>(0,r,s-1) )
        {
            
        }
        
        _Domain:: ~_Domain() throw()
        {
        }
        
    }
}


