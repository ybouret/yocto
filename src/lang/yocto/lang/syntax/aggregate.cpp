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

            const char * Aggregate:: __shp() const throw()
            {
                return "house";
            }

            void Aggregate:: __viz(ios::ostream &fp) const
            {
                __mark(fp);
                fp("[shape=%s,label=\"%s\"];\n",__shp(),label.c_str());
                vizLink(fp);
            }
        }
    }
}
