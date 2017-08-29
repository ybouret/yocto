#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/lang/syntax/rules.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            const string & Grammar:: newAltLabel()
            {
                tmpLabel = vformat("alt#%d",++altCount);
                return tmpLabel;
            }

            const string & Grammar:: newOptLabel()
            {
                tmpLabel = vformat("opt#%d",++optCount);
                return tmpLabel;
            }

            const string & Grammar:: newZomLabel()
            {
                tmpLabel = vformat("*#%d",++zomCount);
                return tmpLabel;
            }

            const string & Grammar:: newOomLabel()
            {
                tmpLabel = vformat("+#%d",++oomCount);
                return tmpLabel;
            }


            Rule & Grammar:: Choice(Rule &a, Rule &b)
            {
                Alternate    &alt   = add( new Alternate(newAltLabel()) );
                return (alt<<a<<b);
            }

            Rule & Grammar:: Choice(Rule &a, Rule &b, Rule &c)
            {
                Alternate    &alt   = add( new Alternate(newAltLabel()) );
                return (alt<<a<<b<<c);
            }

            Rule & Grammar:: Option(Rule &a)
            {
                return add( new Optional(newOptLabel(),a) );
            }

            Rule & Grammar:: ZeroOrMore(Rule &a)
            {
                return add( new Counting(newZomLabel(),a,0) );
            }

            Rule & Grammar:: OneOrMore(Rule &a)
            {
                return add( new Counting(newOomLabel(),a,1) );
            }
        }

    }

}
