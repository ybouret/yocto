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
                YOCTO_LANG_SYNTAX_RULE_SELF();
            }

            const char *Alternate:: __shp() const throw()
            {
                return "egg";
            }

            void Alternate:: __viz(ios::ostream &fp) const
            {
                __mark(fp);
                fp("[shape=%s,label=\"%s\"];\n",__shp(),label.c_str());
            }

            void Alternate:: __lnk(ios::ostream &fp) const
            {
                vizLink(fp);
            }

            bool Alternate:: admit(YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS) const
            {
                YOCTO_LANG_SYNTAX(std::cerr << "?ALT '" << label << "'" << std::endl);
                ++depth;
                for(const MetaNode *m = members.head; m; m=m->next)
                {
                    const Rule &rule = * (m->addr);
                    Node       *node = NULL;
                    if(rule.admit(node,lexer,source,depth))
                    {
                        --depth;
                        YOCTO_LANG_SYNTAX(std::cerr << "|_ok '" << label << "'" << std::endl;);
                        if(node)
                        {
                            Node::Grow(tree,node);
                        }
                        return true;
                    }
                }
                --depth;
                YOCTO_LANG_SYNTAX(std::cerr << "|_no '" << label << "'" << std::endl;);
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
