#include "yocto/lang/pattern/joker.hpp"
#include "yocto/lang/pattern/logic.hpp"
#include "yocto/lang/pattern/basic.hpp"


namespace yocto
{
    namespace Lang
    {

        static inline
        Pattern * __OptimizeAND( Pattern *p ) throw()
        {
            assert(AND::UUID==p->uuid);
            assert(NULL!=p->addr);
            Logical *q = static_cast<Logical *>(p->addr);

            if(1==q->operands.size)
            {
                Pattern *ans = q->operands.pop_back();
                delete   q;
                return   ans;
            }
            else
            {
                // Merge ANDs
                Patterns stk;
                while(q->operands.size>0)
                {
                    Pattern *sub = q->operands.pop_front();
                    if(AND::UUID==sub->uuid)
                    {
                        stk.merge_back( static_cast<Logical *>(sub->addr)->operands );
                        delete sub;
                    }
                    else
                    {
                        stk.push_back(sub);
                    }
                }
                q->operands.swap_with(stk);
            }

            return p;
        }

        Pattern * Pattern:: Optimize( Pattern *p ) throw()
        {
            assert(p);
            switch(p->uuid)
            {
                case AND::UUID:
                    return __OptimizeAND(p);

                default:
                    break;
            }
            return p;
        }
    }
}
