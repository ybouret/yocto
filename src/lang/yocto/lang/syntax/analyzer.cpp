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
                        (size_t &)max_size = max_of(max_size,(*i).size());
                    }
                    termHash.optimize();
                }



                {
                    int h = RulesShift;
                    for( list<string>::iterator i=rules.begin(); i != rules.end(); ++i, ++h)
                    {
                        ruleHash.insert(*i,h);
                        //max_size = max_of(max_size,(*i).size());
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


            void Analyzer:: walk(const Node *tree)
            {
                (int&)depth = 0;
                if(tree)
                {
                    __walk(tree);
                }
            }

            std::ostream & Analyzer:: __indent() const
            {
                for(int i=0;i<depth;++i) std::cerr << "  ";
                return std::cerr;
            }

            std::ostream & Analyzer:: __output(const string &label) const
            {
                std::cerr << label;
                for(size_t i=label.size();i<max_size;++i) std::cerr << ' ';
                return std::cerr;
            }

            void Analyzer:: onTerminal(const string &label,
                                       const int     hCode,
                                       const string &content)
            {
                const string hText = vformat("[%04d]",hCode);
                __indent() << "push " << hText << ' ';
                __output(label);
                if(content.size()>0)
                {
                    std::cerr << " : '" << content << "'";
                }
                std::cerr << std::endl;
            }

            void Analyzer:: onInternal(const string &label,
                                       const int     hCode,
                                       const size_t  nArgs)
            {
                const string hText = vformat("[%04d]",hCode);
                __indent() << "call " << hText << ' ' << label << "/" << nArgs << std::endl;

            }


            void Analyzer:: __walk(const Node *node)
            {
                assert(node);
                const string &label = node->origin.label;
                if(node->terminal)
                {
                    const int    termCode = termHash(label);
                    const string content  = node->toString();
                    onTerminal(label,termCode,content);
                }
                else
                {
                    const int    ruleCode = ruleHash(label);
                    ++( (int&)depth );
                    const Node::List &children = node->toList();
                    for(const Node *ch = children.head; ch; ch=ch->next)
                    {
                        __walk(ch);
                    }
                    --( (int&)depth );
                    onInternal(label,ruleCode,children.size);
                }
            }

#if 0
            void Analyzer:: __indent(ios::ostream &fp) const
            {
                for(int i=0;i<depth;++i) fp << ' ' << ' ';
            }
            
            void Analyzer:: __align(const string &label, ios::ostream &fp) const
            {
                fp << label; for(size_t i=label.size();i<max_size;++i) fp << ' ';
            }
#endif
        }
    }
}
