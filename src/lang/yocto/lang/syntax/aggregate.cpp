#include "yocto/lang/syntax/aggregate.hpp"
namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            Aggregate:: ~Aggregate() throw()
            {

            }

            Aggregate:: Aggregate(const string &id) :
            Compound(id,UUID)
            {
            }

            void Aggregate:: __viz(ios::ostream &fp) const
            {
                __mark(fp);
                fp("[shape=house,label=\"%s\"];\n",label.c_str());
                vizLink(fp);
            }
        }
    }
}
