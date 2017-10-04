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
                YOCTO_LANG_SYNTAX_RULE_SELF(Aggregate);
            }

            Aggregate:: Aggregate(const char *id) :
            Compound(id,UUID)
            {
                YOCTO_LANG_SYNTAX_RULE_SELF(Aggregate);
            }

            
        


            const char * Aggregate:: __shp() const throw()
            {
                return "house";
            }

            void Aggregate:: __viz(ios::ostream &fp) const
            {
                __mark(fp);
                fp("[shape=%s,label=\"%s\",style=\"filled,%s\"];\n",__shp(),*label,__sty());
                //vizLink(fp);
            }

            void Aggregate:: __lnk(ios::ostream &fp) const
            {
                vizLink(fp);
            }

            bool Aggregate:: admit( YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS ) const
            {
                YOCTO_LANG_SYNTAX(std::cerr << "<?AGG '" << label << "', #members=" << getMinCount() << ".." << members.size << ">" << std::endl);
                //______________________________________________________________
                //
                // create a subTree
                //______________________________________________________________
                Node          *subTree = Node::Create(source,*this);
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
                        YOCTO_LANG_SYNTAX(std::cerr << "<-AGG '" << label << "'>" << std::endl);
                        Restore(lexer,pTree.yield());
                        return false;
                    }

                }

                //______________________________________________________________
                //
                // admitted !
                //______________________________________________________________
                --depth;
                YOCTO_LANG_SYNTAX(std::cerr << "<+AGG '" << label << "'>" << std::endl);
                Node::Grow(tree,pTree.yield());
                return true;
            }

            bool Aggregate:: admitsEmpty() const throw()
            {
                // false if at least one of the members doesn't admit an empty token
                for(const MetaNode *m=members.head;m;m=m->next)
                {
                    if( !(m->addr->admitsEmpty() ) ) return false;
                }
                return true;
            }

            size_t Aggregate:: getMinCount() const throw()
            {
                size_t ans = 0;
                for(const MetaNode *m=members.head;m;m=m->next)
                {
                    if( !(m->addr->admitsEmpty() ) )
                    {
                        ++ans;
                    }
                }
                return ans;
            }


        }
    }
}
