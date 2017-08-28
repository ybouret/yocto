#include "yocto/lang/syntax/counting.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            Counting:: ~Counting() throw()
            {
            }

            Counting:: Counting(const string &id,
                                Rule         &r,
                                const size_t  n) :
            Joker(id,UUID,r),
            minCount(n)
            {
            }

            void Counting:: __viz(ios::ostream &fp) const
            {
                __mark(fp); fp("[shape=invhouse,label=\"%s(>=%u)\"];\n",label.c_str(),unsigned(minCount));
                rule.__viz(fp);
                __mark(fp); fp << "->"; rule.__mark(fp); fp << ";\n";
            }
            
        }
    }
}
