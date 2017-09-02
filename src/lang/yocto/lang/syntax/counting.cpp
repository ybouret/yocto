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
                YOCTO_LANG_SYNTAX_RULE_SELF();
            }

            const char * Counting::__shp() const throw()
            {
                return "invhouse";
            }

            void Counting:: __viz(ios::ostream &fp) const
            {
                __mark(fp); fp("[shape=%s,label=\"%s\"];\n",__shp(),label.c_str());
            }

            void Counting:: __lnk(ios::ostream &fp) const
            {
                __mark(fp); fp << "->"; rule.__mark(fp); fp << ";\n";
            }

            bool Counting:: admit( YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS ) const
            {
                YOCTO_LANG_SYNTAX(std::cerr << "?Counting '" << label << "' (>=" << minCount << ")" << std::endl);
                Node *subTree = Node::Create(*this);
                auto_ptr<Node> guard(subTree);
                size_t count = 0;
                ++depth;
                while( rule.admit(subTree,lexer,source,depth) )
                {
                    ++count;
                }
                --depth;
                if(count>=minCount)
                {
                    YOCTO_LANG_SYNTAX(std::cerr << "|_ok '" << label << "', count=" << count << std::endl);
                    Node::Grow(tree,guard.yield());
                    return true;
                }
                else
                {
                    YOCTO_LANG_SYNTAX(std::cerr << "|_no '" << label << "', count=" << count << std::endl);
                    Restore(lexer,guard.yield());
                    return false;
                }
            }

            bool Counting:: admitsEmpty() const throw()
            {
                return (minCount<=0) || (rule.admitsEmpty());
            }

        }
    }
}
