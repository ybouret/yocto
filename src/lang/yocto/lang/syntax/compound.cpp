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

            Compound :: Compound(const char   *id, const uint32_t t ) :
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
                int iNode = 1;
                for(const MetaNode *node = members.head; node; node=node->next, ++iNode)
                {
                    __mark(fp); fp << " -> "; node->addr->__mark(fp);
                    if(members.size>1)
                    {
                        fp("[label=\"%d\"]",iNode);
                    }
                    fp << ";\n";
                }
            }


        }
    }
}
