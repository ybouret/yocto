#include "yocto/lang/syntax/analyzer.hpp"
#include "yocto/sequence/list.hpp"
#include "yocto/code/utils.hpp"

#if 1 == YOCTO_LANG_SYNTAX_ANALYZER_OUTPUT
#include "yocto/ios/graphviz.hpp"
#endif

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            Analyzer:: ~Analyzer() throw() {}


            Analyzer:: Analyzer(const Grammar &G):
            termHash(),
            ruleHash(),
            depth(0),
            max_size(0)
            {


                list<string> terms;
                list<string> rules;
                G.collectLabels(terms,rules);

                {
                    int h = 0;
                    for( list<string>::iterator i=terms.begin(); i != terms.end(); ++i, ++h)
                    {
                        termHash.insert(*i,h);
                        max_size = max_of(max_size,(*i).size());
                    }
                    termHash.optimize();
                }



                {
                    int h = RulesShift;
                    for( list<string>::iterator i=rules.begin(); i != rules.end(); ++i, ++h)
                    {
                        ruleHash.insert(*i,h);
                        max_size = max_of(max_size,(*i).size());
                    }
                    const Rule *topLevel = G.getTopLevel();
                    if(topLevel&&ruleHash(topLevel->label)<0)
                    {
                        ruleHash.insert(topLevel->label,++h);
                    }
                    ruleHash.optimize();
                }

#if 1 == YOCTO_LANG_SYNTAX_ANALYZER_OUTPUT
                std::cerr << "== rendering " << G.name << " terms and rules" << std::endl;
                termHash.graphviz("terms.dot");
                ios::graphviz_render("terms.dot");
                ruleHash.graphviz("rules.dot");
                ios::graphviz_render("rules.dot");
#endif

            }


            void Analyzer:: walk(const Node *tree, ios::ostream *pfp)
            {
                assert(tree);
                depth = 0;
                __walk(tree,pfp);
            }

            void Analyzer:: __walk(const Node *node, ios::ostream *pfp)
            {
                assert(node);
                const string &label = node->origin.label;

                if(node->terminal)
                {
                    const int    termCode = termHash(label);
                    const string content  = node->toString();
                    if(pfp)
                    {
                        ios::ostream &fp = *pfp;
                        __indent(fp);
                        fp("push [%04d] ",termCode);
                        __emit(label,fp);
                        fp << '\'' << content << '\'' << '\n';
                    }
                }
                else
                {
                    const int    ruleCode = ruleHash(label);

                    ++depth;
                    const Node::List &children = node->toList();
                    for(const Node *ch = children.head; ch; ch=ch->next)
                    {
                        __walk(ch,pfp);
                    }
                    --depth;
                    if(pfp)
                    {
                        ios::ostream &fp = *pfp;
                        __indent(fp);
                        fp("call [%04d]",ruleCode);
                        __emit(label,fp);
                        fp("/%u",unsigned(children.size));
                        fp << '\n';
                    }

                }
            }

            void Analyzer:: __indent(ios::ostream &fp) const
            {
                for(int i=0;i<depth;++i) fp << ' ' << ' ';
            }
            
            void Analyzer:: __emit(const string &label, ios::ostream &fp) const
            {
                fp << label; for(size_t i=label.size();i<max_size;++i) fp << ' '; fp << ':';
            }
            
        }
    }
}
