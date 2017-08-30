#include "yocto/lang/syntax/alternate.hpp"
namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            Alternate:: ~Alternate() throw()
            {

            }

            Alternate:: Alternate(const string &id) :
            Compound(id,UUID)
            {
            }

            const char *Alternate:: __shp() const throw()
            {
                return "egg";
            }

            void Alternate:: __viz(ios::ostream &fp) const
            {
                __mark(fp);
                fp("[shape=%s,label=\"%s\"];\n",__shp(),label.c_str());
                vizLink(fp);
            }

            bool Alternate:: admit(YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS) const
            {
                for(const MetaNode *m = members.head; m; m=m->next)
                {
                    const Rule &rule = * (m->addr);
                    Node       *node = NULL;
                    if(rule.admit(node,lexer,source))
                    {
                        if(node)
                        {
                            Node::Grow(tree,node);
                        }
                        return true;
                    }
                }
                return false;
            }

            bool Alternate:: admitsEmpty() const throw()
            {
                for(const MetaNode *m = members.head; m; m=m->next)
                {
                    if(m->addr->admitsEmpty()) return true;
                }
                return false;
            }

        }
    }
}
