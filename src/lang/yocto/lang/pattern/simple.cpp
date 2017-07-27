#include "yocto/lang/pattern/simple.hpp"

namespace yocto
{

    namespace Lang
    {
        Any1:: ~Any1() throw()
        {
        }

        Any1::Any1() throw() : yocto::Lang::Pattern(UUID)
        {
        }

        Any1:: Any1(const Any1 &other) throw() :
        Pattern(other)
        {
        }

        bool Any1:: match(Source &source, Token &result) const
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
    }
}
