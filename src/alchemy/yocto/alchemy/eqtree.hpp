#ifndef YOCTO_ALCHEMY_EQTREE_INCLUDED
#define YOCTO_ALCHEMY_EQTREE_INCLUDED 1

#include "yocto/alchemy/equilibria.hpp"

namespace yocto
{
    namespace alchemy
    {
        class   eqNode;
        typedef core::list_of_cpp<eqNode> eqList;

        class eqNode : public object
        {
        public:
            const equilibrium::pointer pEq;
            eqNode                    *parent;
            eqNode                    *next;
            eqNode                    *prev;
            eqList                     children;

            explicit eqNode(const equilibrium::pointer &p) throw();
            virtual ~eqNode() throw();

            static eqNode *build( const equilibria &eqs );

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(eqNode);
        };



    }
}


#endif
