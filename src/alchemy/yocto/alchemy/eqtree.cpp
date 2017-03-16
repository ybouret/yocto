#include "yocto/alchemy/eqtree.hpp"
#include "yocto/sort/merge.hpp"
#include "yocto/comparator.hpp"

namespace yocto
{
    namespace alchemy
    {
        eqNode:: eqNode( const equilibrium::pointer &p ) throw() :
        pEq(p),
        parent(0),
        next(0),
        prev(0),
        children()
        {
        }

        eqNode:: ~eqNode() throw()
        {
        }


        static inline
        int eqNodeCompare( const eqNode *lhs, const eqNode *rhs, void *)
        {
            return __compare<size_t>(lhs->pEq->number_of_species(),rhs->pEq->number_of_species());
        }

        eqNode * eqNode:: build( const equilibria &eqs )
        {
            std::cerr << std::endl << "---> EQ TREE" << std::endl;
            eqList eql;
            for( equilibria::const_iterator i = eqs.begin(); i != eqs.end(); ++i)
            {
                eql.push_back( new eqNode(*i) );
            }
            std::cerr << "RAW" << std::endl;
            for( const eqNode *node = eql.head; node; node=node->next)
            {
                std::cerr << "\t" << *(node->pEq) << std::endl;
            }

            core::merging<eqNode>::sort(eql,eqNodeCompare,0);
            std::cerr << "SRT" << std::endl;
            for( const eqNode *node = eql.head; node; node=node->next)
            {
                std::cerr << "\t" << *(node->pEq) << std::endl;
            }
            std::cerr  << "<--- EQ TREE" << std::endl << std::endl;

            switch(eql.size)
            {
                case 0:
                    return 0;

                case 1:
                    return eql.pop_back();

                default:
                    break;
            }

            assert(eql.size>1);

            eqNode *root = eql.pop_front();
            


            return root;
        }


    }
}

