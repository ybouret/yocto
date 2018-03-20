#include "yocto/lang/syntax/analyzer.hpp"
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


            void  Analyzer:: __updateMaxSize(const size_t n) const throw()
            {
                (size_t &)max_size = max_of(max_size,n);
            }

            Analyzer:: Analyzer(const Grammar &G):
            terminalHash(),
            internalHash(),
            terminals(),
            internals(),
            depth(0),
            max_size(0)
            {


                G.collectLabels(terminals,internals);

                {
                    int h = 0;
                    for( list<string>::iterator i=terminals.begin(); i != terminals.end(); ++i, ++h)
                    {
                        const string &kw = *i;
                        terminalHash.insert(kw,h);
                        __updateMaxSize(kw.size());
                    }
                    terminalHash.optimize();
                }



                {
                    int h = RulesShift;
                    for( list<string>::iterator i=internals.begin(); i != internals.end(); ++i, ++h)
                    {
                        const string &kw = *i;
                        internalHash.insert(kw,h);
                        __updateMaxSize(kw.size());
                    }

                    const Rule  *topLevel = G.getTopLevel();

                    if(topLevel&&internalHash(topLevel->label)<0)
                    {
                        internals.push_back(topLevel->label);
                        internalHash.insert(topLevel->label,++h);
                        __updateMaxSize(topLevel->label.size());
                    }
                    internalHash.optimize();
                }

#if 0
                int (*cmp)(const string &,const string&) = string::compare;
                terminals.sort( cmp );
                internals.sort( cmp );
#endif

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

            void Analyzer:: showTerminal(const string &label,
                                         const int     hCode,
                                         const string &content) const
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


            void Analyzer:: onTerminal(const string &label,
                                       const int     hCode,
                                       const string &content)
            {
                showTerminal(label,hCode,content);
            }

            void Analyzer:: showInternal(const string &label,
                                         const int     hCode,
                                         const int     nArgs) const
            {
                const string hText = vformat("[%04d]",hCode);
                __indent() << "call " << hText << ' ' << label << "/" << nArgs << std::endl;

            }

            void Analyzer:: onInternal(const string &label,
                                       const int     hCode,
                                       const int     nArgs)
            {
                showTerminal(label,hCode,nArgs);
            }


            void Analyzer:: __walk(const Node *node)
            {
                assert(node);
                const string &label = node->origin.label;
                if(node->terminal)
                {
                    const int    termCode = terminalHash(label);
                    const string content  = node->toString();
                    onTerminal(label,termCode,content);
                }
                else
                {
                    const int    ruleCode = internalHash(label);
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


            static inline
            void __Emit(ios::ostream       &fp,
                        const Grammar      &G,
                        const list<string> &keywords,
                        const Hasher       &H,
                        const size_t        max_size)
            {
                for( list<string>::const_iterator i = keywords.begin(); i != keywords.end(); ++i)
                {
                    const string &kw = *i;
                    const Rule   &r  = G.getRuleByLabel(kw);
                    fp << "#define ";
                    if(r.tag.is_valid()) { fp << *r.tag << '_'; }
                    for(size_t j=0;j<kw.size();++j)
                    {
                        char c = kw[j];
                        if( !isalnum(c)  )
                        {
                            c = '_';
                        }
                        fp << c;
                    }
                    for(size_t j=kw.length();j<max_size;++j) fp << ' ';
                    fp(" %d\n", H(kw));
                }

            }

            void Analyzer:: emitDefinitions( ios::ostream &fp, const Grammar &G) const
            {
                fp << "// TERMINALS\n";
                //std::cerr << "emitting terminals..." << std::endl;
                __Emit(fp,G,terminals,terminalHash,max_size);

                fp << "// INTERNALS\n";
                //std::cerr << "emitting internal..." << std::endl;
                __Emit(fp,G,internals,internalHash,max_size);

            }

        }
    }
}
