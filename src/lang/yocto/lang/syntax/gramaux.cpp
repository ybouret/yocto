#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/lang/syntax/rules.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            const string & Grammar:: newAggLabel()
            {
                tmpLabel = vformat("agg%c%d",Rule::FusionMark,++aggCount);
                return tmpLabel;
            }

            const string & Grammar:: newAltLabel()
            {
                tmpLabel = vformat("alt%c%d",Rule::FusionMark,++altCount);
                return tmpLabel;
            }

            const string & Grammar:: newOptLabel()
            {
                tmpLabel = vformat("opt%c%d",Rule::FusionMark,++optCount);
                return tmpLabel;
            }

            const string & Grammar:: newZomLabel()
            {
                tmpLabel = vformat("*%c%d",Rule::FusionMark,++zomCount);
                return tmpLabel;
            }

            const string & Grammar:: newOomLabel()
            {
                tmpLabel = vformat("+%c%d",Rule::FusionMark,++oomCount);
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
