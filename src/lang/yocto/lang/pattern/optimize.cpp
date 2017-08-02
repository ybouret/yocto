#include "yocto/lang/pattern/joker.hpp"
#include "yocto/lang/pattern/logic.hpp"
#include "yocto/lang/pattern/basic.hpp"
#include "yocto/sort/merge.hpp"

#include <iostream>
namespace yocto
{
    namespace Lang
    {

        static inline
        int compareSingles(const Pattern *lhs, const Pattern *rhs, void *) throw()
        {
            assert(lhs->addr);
            assert(rhs->addr);
            const int L( static_cast<const Single *>(lhs)->code );
            const int R( static_cast<const Single *>(rhs)->code );
            return L-R;
        }

        static inline
        void __NoMultipleSinglesIn( Patterns &ops ) throw()
        {
            Patterns singles;
            Patterns others;
            while(ops.size)
            {
                Pattern *p = ops.pop_front();
                if(Single::UUID == p->uuid )
                {
                    singles.push_back(p);
                }
                else
                {
                    others.push_back(p);
                }
            }
            core::merging<Pattern>::sort(singles, compareSingles, NULL);
            {
                Patterns tmp;
                while(singles.size>0)
                {
                    Pattern *p = singles.pop_front();
                    if(tmp.tail)
                    {
                        const int cmp =compareSingles(tmp.tail,p,NULL);
                        if(cmp<0)
                        {
                            tmp.push_back(p);
                        }
                        else
                        {
                            delete p;
                        }
                    }
                    else
                    {
                        tmp.push_back(p);
                    }
                }
                singles.swap_with(tmp);
            }
            ops.merge_back(singles);
            ops.merge_back(others);
        }

        //______________________________________________________________________
        //
        // optimizing AND
        //______________________________________________________________________
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
                    Pattern *sub = Pattern::Optimize(q->operands.pop_front());
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

        
        //______________________________________________________________________
        //
        // optimizing OR
        //______________________________________________________________________
        static inline
        Pattern * __OptimizeOR( Pattern *p ) throw()
        {
            assert(OR::UUID==p->uuid);
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
                    Pattern *sub = Pattern::Optimize(q->operands.pop_front());
                    if(OR::UUID==sub->uuid)
                    {
                        stk.merge_back( static_cast<Logical *>(sub->addr)->operands );
                        delete sub;
                    }
                    else
                    {
                        stk.push_back(sub);
                    }
                }
                __NoMultipleSinglesIn(stk);
                q->operands.swap_with(stk);
            }
            
            return p;
        }

        //______________________________________________________________________
        //
        // optimizing NONE
        //______________________________________________________________________
        static inline
        Pattern * __OptimizeNONE( Pattern *p ) throw()
        {
            assert(NONE::UUID==p->uuid);
            assert(NULL!=p->addr);
            Logical *q = static_cast<Logical *>(p->addr);
            // Merge NONEs
            Patterns stk;
            while(q->operands.size>0)
            {
                Pattern *sub = Pattern::Optimize(q->operands.pop_front());
                if(NONE::UUID==sub->uuid)
                {
                    stk.merge_back( static_cast<Logical *>(sub->addr)->operands );
                    delete sub;
                }
                else
                {
                    stk.push_back(sub);
                }
            }
            __NoMultipleSinglesIn(stk);
            q->operands.swap_with(stk);
            return p;
        }

        Pattern * Pattern:: Optimize( Pattern *p ) throw()
        {
            assert(p);
            switch(p->uuid)
            {
                case AND::UUID:
                    return __OptimizeAND(p);

                case OR::UUID:
                    return __OptimizeOR(p);

                case NONE::UUID:
                    return __OptimizeNONE(p);

                case Optional:: UUID:
                case AtLeast::  UUID:
                case Counting:: UUID:
                    static_cast<Joker *>(p->addr)->optimize();
                    return p;

                default:
                    break;
            }
            return p;
        }
    }
}
