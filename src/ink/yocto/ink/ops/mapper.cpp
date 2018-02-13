#include "yocto/ink/ops/mapper.hpp"

namespace yocto
{
    namespace Ink
    {
        Mapper::  Mapper() throw() :
        target(0),
        source(0),
        lhs(0),
        rhs(0),
        proc(0)
        {}

        Mapper:: ~Mapper() throw() {}
        
    }
}


