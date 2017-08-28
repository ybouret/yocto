#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/exception.hpp"
#include "yocto/ios/ocstream.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            Grammar:: ~Grammar() throw()
            {
            }

            Grammar:: Grammar(const string &id) :
            name(id),
            rules(),
            altCount(0),
            optCount(0),
            zomCount(0),
            oomCount(0),
            tmpLabel()
            {
            }

            void Grammar:: append( Rule *rule )
            {
                assert(rule);
                for(const Rule *r=rules.head;r;r=r->next)
                {
                    if(r->label==rule->label) throw exception("{%s} multiple rule '%s'", name.c_str(), r->label.c_str() );
                }
                rules.push_back(rule);
            }

            void Grammar:: setTopLevel(Rule &rule)
            {
                rules.move_to_front( &rule );
            }

            Rule * Grammar:: getTopLevel() const throw()
            {
                return rules.head;
            }


            Node * Grammar:: accept(Lexer &lexer, Source &source)
            {
                const char *gramID = name.c_str();
                if(rules.size<=0) throw exception("{%s} no top level rule", gramID);

                Node *tree = 0;
                Rule &top  = * rules.head;

                if(!top.admit(tree,lexer,source))
                {
                    throw exception("{%s} couldn't accept '%s'", gramID , top.label.c_str());
                }

                return tree;
            }

            void Grammar:: graphviz(const string &filename) const
            {
                ios::wcstream fp(filename);
                fp << "digraph G {\n";
                if(rules.head)
                {
                    rules.head->__viz(fp);
                }
                fp << "}\n";
            }

        }
    }
}
