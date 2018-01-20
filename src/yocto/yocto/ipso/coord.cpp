#include "yocto/ipso/coord.hpp"
#include "yocto/string/conv.hpp"


namespace yocto
{
    namespace ipso
    {
        

        coord1D __coord_parser:: string2coord(const string &word, const string &label)
        {
            return strconv::to_int(word,*label);
        }

        bool __coord_parser:: is_sep(char C) throw() { return ',' == C; }
        bool __coord_parser:: is_bad(char C) throw() { return ' ' == C || '\t' == C; }

    }
}

