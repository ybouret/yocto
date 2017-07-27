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

        Logical & Logical:: operator<<( Pattern *p )
        {
            assert(p);
            operands.push_back(p);
            return *this;
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

        bool AND:: match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            // loop over all operands
            for(const Pattern *op = operands.head;op;op=op->next)
            {
                // local result
                Token res;
                if(op->match(source,res))
                {
                    result.merge_back(res);
                }
                else
                {
                    assert(0==res.size);
                    source.store(result);
                    return false;
                }
            }
            return true;
        }

    }
}


namespace yocto
{
    namespace Lang
    {
        OR:: OR() throw() : Logical(UUID)
        {
        }

        OR::OR(const OR &other) :
        Logical(other)
        {
        }

        bool OR:: match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            // loop over all operands
            for(const Pattern *op = operands.head;op;op=op->next)
            {
                assert(0==result.size);
                if(op->match(source,result))
                {
                    return true;
                }
                assert(0==result.size);
            }
            return false;
        }
        
    }
}

