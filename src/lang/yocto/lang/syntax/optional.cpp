#include "yocto/lang/syntax/optional.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            Optional:: ~Optional() throw()
            {
            }

            Optional:: Optional(const string &id,
                                Rule         &r) :
            Joker(id,UUID,r)
            {
            }

            void Optional:: __viz(ios::ostream &fp) const
            {
                __mark(fp); fp("[shape=circle,label=\"%s\"];\n",label.c_str());
                rule.__viz(fp);
                __mark(fp); fp << "->"; rule.__mark(fp); fp << ";\n";
            }

        }
    }
}
