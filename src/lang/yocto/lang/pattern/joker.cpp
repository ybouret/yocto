#include "yocto/lang/pattern/joker.hpp"

namespace yocto
{
    namespace Lang
    {
        Joker:: ~Joker() throw()
        {
            assert(motif);
            delete motif;
            motif=0;
        }

        Joker::Joker(const uint32_t t,const Pattern *p) throw() :
        Pattern(t),
        motif(p)
        {
            assert(motif);
        }

        Joker:: Joker(const Joker &other) :
        Pattern(other),
        motif(other.motif->clone())
        {

        }
    }

}

#include "yocto/ptr/auto.hpp"

namespace yocto
{
    namespace Lang
    {
        Optional:: Optional(const Optional &other):
        Joker(other)
        {
        }


        Optional:: Optional(const Pattern *p) throw() :
        Joker(UUID,p)
        {

        }

        Pattern *Optional:: Create(Pattern *p)
        {
            assert(p);
            auto_ptr<Pattern> guard(p);
            Pattern *opt = new Optional(p);
            guard.forget();
            return opt;
        }

        bool Optional::match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            if(motif->match(source,result))
            {
                return true;
            }
            else
            {
                return true;
            }
        }
    }

}

namespace yocto
{
    namespace Lang
    {
        AtLeast:: AtLeast(const Pattern *p,
                          const size_t   n) throw() :
        Joker(UUID,p),
        count(n)
        {

        }

        AtLeast:: AtLeast(const AtLeast &other) :
        Joker(other),
        count(other.count)
        {
        }

        Pattern * AtLeast:: Create( Pattern *p, const size_t n)
        {
            assert(p);
            auto_ptr<Pattern> guard(p);
            Pattern *q = new AtLeast(p,n);
            guard.forget();
            return q;
        }

        bool AtLeast::match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            size_t num = 0;
            while(true)
            {
                Token res;
                if(motif->match(source,res))
                {
                    ++num;
                    result.merge_back(res);
                }
                else
                {
                    break;
                }
            }
            
            if(num>=count)
            {
                return true;
            }
            else
            {
                source.store(result);
                return false;
            }
        }

    }

}

