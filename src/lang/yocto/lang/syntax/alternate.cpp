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
                YOCTO_LANG_SYNTAX_RULE_SELF(Alternate);
            }

            Alternate:: Alternate(const char *id) :
            Compound(id,UUID)
            {
                YOCTO_LANG_SYNTAX_RULE_SELF(Alternate);
            }

            const char *Alternate:: __shp() const throw()
            {
                return "egg";
            }

            void Alternate:: __viz(ios::ostream &fp) const
            {
                __mark(fp);
                const char *com = ( tag.is_valid() ? **tag : "" );
                fp("[shape=%s,label=\"%s/%s\"];\n",__shp(),*label,com);
            }

            void Alternate:: __lnk(ios::ostream &fp) const
            {
                vizLink(fp);
            }

            bool Alternate:: admit(YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS) const
            {
                YOCTO_LANG_SYNTAX(std::cerr << "<?ALT '" << label << "', #possibility=" << members.size << ">" << std::endl);
                ++depth;
                size_t winner = 1;
                for(const MetaNode *m = members.head; m; m=m->next,++winner)
                {
                    const Rule &rule = * (m->addr);
                    Node       *node = NULL;
                    if(rule.admit(node,lexer,source,depth))
                    {
                        --depth;
                        YOCTO_LANG_SYNTAX(std::cerr << "<+ALT '" << label << "', choice " << winner << "/" << members.size << " (" << rule.label << ")>" << std::endl;);
                        if(node)
                        {
                            Node::Grow(tree,node);
                        }
                        return true;
                    }
                }
                --depth;
                YOCTO_LANG_SYNTAX(std::cerr << "<-ALT '" << label << "'>" << std::endl;);
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
