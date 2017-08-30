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
            addr = static_cast<Logical *>(this);
        }

        Logical:: Logical( const Logical &other ) :
        Pattern(other),
        operands(other.operands)
        {
            addr = static_cast<Logical *>(this);
        }

        Logical & Logical:: operator<<( Pattern *p )
        {
            assert(p);
            operands.push_back(p);
            return *this;
        }

        void Logical:: __viz_ops( ios::ostream &fp ) const
        {
            for( const Pattern *op = operands.head; op; op=op->next )
            {
                op->__viz(fp);
                __mark(fp); fp << "->"; op->__mark(fp); fp << ";\n";
            }
        }

        void Logical:: __sav_ops( ios::ostream &fp ) const
        {
            fp.emit<uint32_t>(operands.size);
            for( const Pattern *op = operands.head; op; op=op->next )
            {
                op->save(fp);
            }
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

        void AND:: __viz(ios::ostream &fp) const
        {
            __mark(fp);
            fp << "[label=\"&&\",shape=house];\n";
            __viz_ops(fp);
        }

        void AND:: __sav(ios::ostream &fp) const
        {
            __sav_ops(fp);
        }

        bool AND:: allowsEmpty() const throw()
        {
            for(const Pattern *op = operands.head;op;op=op->next)
            {
                if(!op->allowsEmpty())
                {
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

        void OR:: __viz( ios::ostream &fp ) const
        {
            __mark(fp);
            fp << "[label=\"||\",shape=diamond];\n";
            __viz_ops(fp);
        }

        void OR:: __sav(ios::ostream &fp) const
        {
            __sav_ops(fp);
        }

        bool OR:: allowsEmpty() const throw()
        {
            for(const Pattern *op = operands.head;op;op=op->next)
            {
                if(op->allowsEmpty())
                {
                    return true;
                }
            }
            return false;
        }
    }
}


namespace yocto
{
    namespace Lang
    {
        NONE:: NONE() throw() : Logical(UUID)
        {
        }

        NONE::NONE(const NONE &other) :
        Logical(other)
        {
        }

        bool NONE:: match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            // loop over all operands
            for(const Pattern *op = operands.head;op;op=op->next)
            {
                assert(0==result.size);
                if(op->match(source,result))
                {
                    source.store(result);
                    return false;
                }
                assert(0==result.size);
            }
            // do we have something ?
            Char *ch = source.query();
            if(!ch)
            {
                return false;
            }
            {
                result.push_back(ch);
                return true;
            }
        }

        void NONE:: __viz( ios::ostream &fp ) const
        {
            __mark(fp);
            fp << "[label=\"!!\",shape=triangle];\n";
            __viz_ops(fp);
        }

        void NONE:: __sav(ios::ostream &fp) const
        {
            __sav_ops(fp);
        }

        bool NONE:: allowsEmpty() const throw()
        {
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


