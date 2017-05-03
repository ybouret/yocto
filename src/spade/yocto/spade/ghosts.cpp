#include "yocto/spade/ghosts.hpp"

namespace yocto
{
    namespace spade
    {
        ghost_info:: ~ghost_info() throw() {}

        ghost_info:: ghost_info(const int which) throw() : peer(which) {}

        ghost_info:: ghost_info(const ghost_info &other) throw() : peer( other.peer ) {}
        

    }
}
