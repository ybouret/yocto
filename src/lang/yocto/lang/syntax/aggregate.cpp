#include "yocto/lang/syntax/aggregate.hpp"
#include "yocto/exception.hpp"

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
                YOCTO_LANG_SYNTAX_RULE_SELF();
            }

            const char * Aggregate:: __shp() const throw()
            {
                return "house";
            }

            void Aggregate:: __viz(ios::ostream &fp) const
            {
                __mark(fp);
                fp("[shape=%s,label=\"%s\"];\n",__shp(),label.c_str());
                //vizLink(fp);
            }

            void Aggregate:: __lnk(ios::ostream &fp) const
            {
                vizLink(fp);
            }

            bool Aggregate:: admit( YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS ) const
            {
                YOCTO_LANG_SYNTAX(std::cerr << "?AGG '" << label << "'" << std::endl);
                //______________________________________________________________
                //
                // create a subTree
                //______________________________________________________________
                Node          *subTree = Node::Create(*this);
                auto_ptr<Node> pTree(subTree);

                //______________________________________________________________
                //
                // all the rules must be admitted
                //______________________________________________________________
                ++depth;
                for(const MetaNode *m=members.head;m;m=m->next)
                {
                    Rule &rule = *(m->addr);
                    if( !rule.admit(subTree,lexer,source,depth) )
                    {
                        --depth;
                        YOCTO_LANG_SYNTAX(std::cerr << "|_no '" << label << "'" << std::endl);
                        Restore(lexer,pTree.yield());
                        return false;
                    }

                }

                //______________________________________________________________
                //
                // admitted !
                //______________________________________________________________
                --depth;
                YOCTO_LANG_SYNTAX(std::cerr << "|_ok '" << label << "'" << std::endl);
                Node::Grow(tree,pTree.yield());
                return true;
            }

            bool Aggregate:: admitsEmpty() const throw()
            {
                for(const MetaNode *m=members.head;m;m=m->next)
                {
                    if( !(m->addr->admitsEmpty() ) ) return false;
                }
                return true;
            }


        }
    }
}
