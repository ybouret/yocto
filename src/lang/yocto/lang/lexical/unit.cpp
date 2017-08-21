#include "yocto/lang/lexical/unit.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Unit:: ~Unit() throw()
            {
            }


            Unit:: Unit(const Stamp &usrStamp) throw() :
            Token(),
            next(0),
            prev(0),
            stamp(usrStamp),
            label(*stamp)
            {
            }

            Unit:: Unit(const Unit &other) :
            Token(other),
            next(0),
            prev(0),
            stamp(other.stamp),
            label(*stamp)
            {
            }


            void Unit::RemoveFrom( Units &units, const string &id) throw()
            {
                Units tmp;
                while(units.size)
                {
                    Unit *u = units.pop_front();
                    if(id==u->label)
                    {
                        delete u;
                    }
                    else
                    {
                        tmp.push_back(u);
                    }
                }
                units.swap_with(tmp);
            }

        }
    }
}
