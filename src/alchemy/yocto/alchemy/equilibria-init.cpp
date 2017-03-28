#include "yocto/alchemy/equilibria.hpp"


namespace yocto
{
    using namespace math;

    namespace alchemy
    {


        void equilibria:: init0( array<double> &C0 )
        {
            assert(C0.size()>=M);
            static const char fn[] = "equilibria:init0: ";

            //__________________________________________________________________
            //
            // first pass: getting reactants only equilibria
            //__________________________________________________________________
            vector<equilibrium::pointer> sub(N,as_capacity);
            for( iterator it = begin(); it != end(); ++it)
            {
                const equilibrium::pointer &pEq = *it;
                if(pEq->number_of_species()<=0)
                {
                    throw exception("%sinvalid equilibrium",fn);
                }
                if(pEq->get_reactants().size<=0||pEq->get_products().size<=0)
                {
                    std::cerr << "INIT: Using " << pEq->name << std::endl;
                    assert(pEq->get_products().size>0);
                    sub.push_back(pEq);
                }
            }

            //__________________________________________________________________
            //
            // second pass, getting involved species
            //__________________________________________________________________
            vector<species::pointer> isp(M,as_capacity);
            for(size_t i=sub.size();i>0;--i)
            {
                const equilibrium &eq = *sub[i];
                assert(eq.get_products().size>0);
                for(const actor *node = eq.get_products().head; node; node=node->next)
                {
                    std::cerr << "-- will use " << node->sp->name << std::endl;
                    bool has_species = false;
                    for(size_t j=isp.size();j>0;--j)
                    {
                        if(isp[j]->name==node->sp->name)
                        {
                            has_species=true;
                            break;
                        }
                    }
                    if(!has_species)
                    {
                        isp.push_back(node->sp);
                    }
                }
            }

            std::cerr << "summary: " << std::endl;
            for(size_t j=1;j<=isp.size();++j)
            {
                std::cerr << "\t--> " << isp[j]->name << std::endl;
            }
            

#if 0
            //__________________________________________________________________
            //
            // Third pass: gettin reactions with matter exchange and involved
            // species
            //__________________________________________________________________
            sub.free();
            for( iterator it = begin(); it != end(); ++it)
            {
                const equilibrium::pointer &pEq = *it;
                for(size_t j=isp.size();j>0;--j)
                {
                    if(pEq->involves(isp[j]->name) && pEq->delta_nu()!=0 )
                    {
                        sub.push_back(pEq);
                        break;
                    }
                }
            }
            std::cerr << "Involving " << sub.size() << " eqs" << std::endl;
            for(size_t i=1;i<=sub.size();++i)
            {
                std::cerr << "\t\t-->" << sub[i]->name << std::endl;
            }
#endif

        }


    }

}

