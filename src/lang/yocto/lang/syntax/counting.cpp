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

            const char * Counting::__shp() const throw()
            {
                return "invhouse";
            }

            void Counting:: __viz(ios::ostream &fp) const
            {
                __mark(fp); fp("[shape=%s,label=\"%s\"];\n",__shp(),label.c_str());
                rule.__viz(fp);
                __mark(fp); fp << "->"; rule.__mark(fp); fp << ";\n";
            }

            bool Counting:: admit( YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS ) const
            {
                Node *subTree = Node::Create(*this);
                auto_ptr<Node> guard(subTree);
                size_t count = 0;
                while( rule.admit(subTree,lexer,source) )
                {
                    ++count;
                }
                if(count>=minCount)
                {
                    Node::Grow(tree,guard.yield());
                    return true;
                }
                else
                {
                    Restore(lexer,guard.yield());
                    return false;
                }
            }

        }
    }
}
