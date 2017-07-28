#include "yocto/lang/pattern/basic.hpp"
#include "yocto/ios/graphviz.hpp"

namespace yocto
{

    namespace Lang
    {


        Any1::Any1() throw() : yocto::Lang::Pattern(UUID)
        {
        }

        Any1:: Any1(const Any1 &other) throw() :
        Pattern(other)
        {
        }

        bool Any1:: match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            Char *ch = source.query();
            if(ch)
            {
                result.push_back(ch);
                return true;
            }
            else
            {
                return false;
            }
        }

        void Any1:: __viz(ios::ostream &fp) const
        {
            __mark(fp);
            fp << "[label=\"ANY1\",shape=circle];\n";
        }
    }
}


namespace yocto
{

    namespace Lang
    {
       

        Single::Single(const uint8_t C) throw() :
        yocto::Lang::Pattern(UUID),
        code(C)
        {
        }

        Single:: Single(const Single &other) throw() :
        Pattern(other),
        code(other.code)
        {
        }

        bool Single:: match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            Char *ch = source.query();
            if(ch)
            {
                if(code==ch->code)
                {
                    result.push_back(ch);
                    return true;
                }
                else
                {
                    source.store(ch);
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        void Single:: __viz(ios::ostream &fp) const
        {

            __mark(fp);
            fp << "[label=\"";
            ios::graphviz_encode(char(code),fp);
            fp << "\",shape=square];\n";
        }
    }
}

#include "yocto/code/utils.hpp"

namespace yocto
{

    namespace Lang
    {
        Range:: Range(const uint8_t lo, const uint8_t up) throw() :
        Pattern(UUID),
        lower( min_of(lo,up) ),
        upper( max_of(lo,up) )
        {
        }

        Range:: Range(const Range &other) throw() :
        Pattern(other),
        lower(other.lower),
        upper(other.upper)
        {
        }

        bool Range:: match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);

            Char *ch = source.query();
            if(ch)
            {
                if(ch->code>=lower&&ch->code<=upper)
                {
                    result.push_back(ch);
                    return true;
                }
                else
                {
                    source.store(ch);
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        void Range:: __viz(ios::ostream &fp) const
        {
            __mark(fp);
            fp << "[label=\"";
            ios::graphviz_encode(char(lower),fp);
            fp << '-';
            ios::graphviz_encode(char(upper),fp);
            fp << "\",shape=note];\n";
        }
    }

}
