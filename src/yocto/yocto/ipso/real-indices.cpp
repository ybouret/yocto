
#include "yocto/ipso/real-indices.hpp"

namespace yocto {

    namespace ipso
    {
        real_indices:: real_indices() throw() : imin(), imax()
        {
        }

        real_indices:: ~real_indices() throw()
        {
        }

        real_indices:: real_indices( const real_indices &other ) throw() :
        imin(other.imin), imax(other.imax)
        {

        }
        
        real_indices & real_indices:: operator=(const real_indices &other) throw()
        {
            imin = other.imin;
            imax = other.imax;
            return *this;
        }
    }
}
