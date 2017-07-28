#include "yocto/lang/pattern/dict.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        PatternDict:: PatternDict() throw() {}

        PatternDict:: ~PatternDict() throw() {}

        void PatternDict:: define(const string &id, Pattern *p)
        {
            const PatternPtr q(p);
            if(!insert(id,p))
            {
                throw exception("PatternDict: muliple '%s'", id.c_str());
            }
        }

        void PatternDict:: define(const char *id_, Pattern *p)
        {
            const PatternPtr q(p);
            const string     id(id_);
            if(!insert(id,p))
            {
                throw exception("PatternDict: muliple '%s'", id.c_str());
            }
        }
    }
}
