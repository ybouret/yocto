#include "yocto/lang/syntax/compound.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {


            Compound:: ~Compound() throw()
            {
            }

            Compound :: Compound(const string &id, const uint32_t t ) :
            Rule(id,t),
            members()
            {
            }


            Compound & Compound:: operator<<( Rule &r )
            {
                members.append( &r );
                return *this;
            }

            void Compound:: vizLink( ios::ostream &fp ) const
            {
                for(const MetaNode *node = members.head; node; node=node->next )
                {
                    node->addr->__viz(fp);
                    __mark(fp); fp << " -> "; node->addr->__mark(fp); fp << ";\n";
                }
            }


        }
    }
}
