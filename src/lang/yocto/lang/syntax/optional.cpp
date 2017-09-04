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
                YOCTO_LANG_SYNTAX_RULE_SELF();
            }

            const char * Optional:: __shp() const throw()
            {
                return "circle";
            }

            void Optional:: __viz(ios::ostream &fp) const
            {
                __mark(fp); fp("[shape=%s,label=\"%s\"];\n", __shp(), label.c_str());
            }

            void Optional:: __lnk(ios::ostream &fp) const
            {
                __mark(fp); fp << "->"; rule.__mark(fp); fp << ";\n";
            }

            bool Optional:: admit( YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS ) const
            {
                YOCTO_LANG_SYNTAX(std::cerr << "?OPT '" << rule.label << "'" <<std::endl);
                Node *node = 0;
                ++depth;
                if(rule.admit(node,lexer,source,depth))
                {
                    --depth;
                    YOCTO_LANG_SYNTAX(std::cerr << "|_ok '" << rule.label << "'" << std::endl);
                    if(node)
                    {
                        Node::Grow(tree,node);
                    }
                    return true;
                }
                else
                {
                    --depth;
                    YOCTO_LANG_SYNTAX(std::cerr << "|_no '" << rule.label << "' (but returns true)" << std::endl);
                    return true;
                }
            }


            bool Optional:: admitsEmpty() const throw()
            {
                return true;
            }

        }
    }
}
