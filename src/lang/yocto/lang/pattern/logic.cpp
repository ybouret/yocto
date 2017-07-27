#include "yocto/lang/pattern/logic.hpp"
#include "yocto/ptr/auto.hpp"
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

#include <iostream>

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


#include "yocto/lang/pattern/basic.hpp"

namespace yocto
{
    namespace Lang
    {
        Logical *Logical:: Among(const string &chars)
        {
            auto_ptr<Logical> ans( new OR() );
            const size_t n = chars.size();
            for(size_t i=0;i<n;++i)
            {
                ans->operands.push_back( new Single(chars[i]) );
            }
            return ans.yield();
        }

        Logical * Logical::Among(const char   *chars)
        {
            const string ch(chars);
            return Among(ch);
        }

        Logical *Logical:: Equal(const string &chars)
        {
            auto_ptr<Logical> ans( new AND() );
            const size_t n = chars.size();
            for(size_t i=0;i<n;++i)
            {
                ans->operands.push_back( new Single(chars[i]) );
            }
            return ans.yield();

        }

        Logical *Logical:: Equal(const char *chars)
        {
            const string ch(chars);
            return Equal(ch);
        }

    }

}


