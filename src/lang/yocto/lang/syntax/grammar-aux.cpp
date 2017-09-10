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
                tmpLabel = vformat("agg%c%d",Rule::InternalMark,++aggCount);
                return tmpLabel;
            }

            const string & Grammar:: newAltLabel()
            {
                tmpLabel = vformat("alt%c%d",Rule::InternalMark,++altCount);
                return tmpLabel;
            }

            const string & Grammar:: newOptLabel()
            {
                tmpLabel = vformat("opt%c%d",Rule::InternalMark,++optCount);
                return tmpLabel;
            }

            const string & Grammar:: newZomLabel()
            {
                tmpLabel = vformat("*%c%d",Rule::InternalMark,++zomCount);
                return tmpLabel;
            }

            const string & Grammar:: newOomLabel()
            {
                tmpLabel = vformat("+%c%d",Rule::InternalMark,++oomCount);
                return tmpLabel;
            }


            Rule & Grammar:: choice(Rule &a, Rule &b)
            {
                Alternate    &alt   = add( new Alternate(newAltLabel()) );
                return (alt<<a<<b);
            }

            Rule & Grammar:: choice(Rule &a, Rule &b, Rule &c)
            {
                Alternate    &alt   = add( new Alternate(newAltLabel()) );
                return (alt<<a<<b<<c);
            }

            Rule & Grammar:: optional(Rule &a)
            {
                return add( new Optional(newOptLabel(),a) );
            }

            Rule & Grammar:: zeroOrMore(Rule &a)
            {
                return add( new Counting(newZomLabel(),a,0) );
            }

            Rule & Grammar:: oneOrMore(Rule &a)
            {
                return add( new Counting(newOomLabel(),a,1) );
            }
        }

    }

}
