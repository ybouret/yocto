#include "yocto/lang/pattern/joker.hpp"

namespace yocto
{
    namespace Lang
    {
        Joker:: ~Joker() throw()
        {
        }

        Joker::Joker(const uint32_t t,const Handle &p) throw() :
        Pattern(t),
        motif(p)
        {
        }

        Joker:: Joker(const Joker &other) throw():
        Pattern(other),
        motif(other.motif)
        {

        }
    }

}

namespace yocto
{
    namespace Lang
    {
        Optional:: Optional(const Optional &other) throw() :
        Joker(other)
        {
        }

        Optional:: Optional(const Handle &p) throw() :
        Joker(UUID,p)
        {
        }

        bool Optional::match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            if(motif->match(source,result))
            {
                return true;
            }
            return true;
        }
    }

}
