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
            }

            const char * Optional:: __shp() const throw()
            {
                return "circle";
            }

            void Optional:: __viz(ios::ostream &fp) const
            {
                __mark(fp); fp("[shape=%s,label=\"%s\"];\n", __shp(), label.c_str());
                rule.__viz(fp);
                __mark(fp); fp << "->"; rule.__mark(fp); fp << ";\n";
            }

            bool Optional:: admit( YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS ) const
            {
                Node *node = 0;
                if(rule.admit(node, lexer, source))
                {
                    Node::Grow(tree,node);
                }
                return true;
            }

        }
    }
}
