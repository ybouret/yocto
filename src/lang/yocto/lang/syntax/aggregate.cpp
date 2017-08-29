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

            bool Aggregate:: admit( YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS ) const
            {
                Node          *subTree = Node::Create(*this);
                auto_ptr<Node> guard(subTree);
                for(const MetaNode *m=members.head;m;m=m->next)
                {
                    Rule &rule = *(m->addr);
                    if( !rule.admit(subTree,lexer,source) )
                    {
                        Restore(lexer,guard.yield());
                        return false;
                    }
                }
                Node::Grow(tree,guard.yield());
                return true;
            }
        }
    }
}
