#include "yocto/lang/pattern/logic.hpp"

namespace yocto
{
    namespace Lang
    {
        Logical:: ~Logical() throw()
        {
        }

        Logical:: Logical(const uint32_t t) throw() :
        Pattern(t),
        operands()
        {
        }

        Logical:: Logical( const Logical &other ) :
        Pattern(other),
        operands(other.operands)
        {
        }
        
    }

}

namespace yocto
{
    namespace Lang
    {
        AND:: AND() throw() : Logical(UUID)
        {
        }

        AND::AND(const AND &other) :
        Logical(other)
        {
        }
        
    }
}
