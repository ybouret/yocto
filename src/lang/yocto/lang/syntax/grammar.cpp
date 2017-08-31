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
            aggCount(0),
            tmpLabel()
            {
            }

            void Grammar:: append( Rule *rule )
            {
                assert(rule);
                for(const Rule *r=rules.head;r;r=r->next)
                {
                    if(r->label==rule->label)
                    {
                        delete rule;
                        throw exception("{%s} multiple rule '%s'", name.c_str(), r->label.c_str() );
                    }
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


          
            void Grammar:: graphviz(const string &filename) const
            {
                ios::wcstream fp(filename);
                fp << "digraph G {\n";
                // write single rules
                for(const Rule *rule = rules.head; rule; rule=rule->next)
                {
                    rule->__viz(fp);
                }
                // link them
                for(const Rule *rule = rules.head; rule; rule=rule->next)
                {
                    rule->__lnk(fp);
                }
                fp << "}\n";
            }

            Alternate & Grammar:: alt()
            {
                return add( new Alternate( newAltLabel() ) );
            }

            Alternate & Grammar:: alt(const string &label)
            {
                return add( new Alternate(label) );
            }

            Alternate & Grammar:: alt(const char *label)
            {
                const string __label(label);
                return add( new Alternate(__label) );
            }



            Aggregate & Grammar:: agg( const string &label )
            {
                return add( new Aggregate(label) );
            }

            Aggregate & Grammar:: agg( const char *label )
            {
                const string __label(label);
                return add( new Aggregate(__label) );
            }


        }
    }
}
