#include "yocto/lang/lexical/plugin/comment.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            Comment:: ~Comment() throw() {}

            Comment:: Comment(const char *id, const char *expr) :
            Plugin(id),
            data(expr)
            {
            }

            const char * Comment:: trigger() const throw()
            {
                return data.c_str();
            }

        }

    }

}
