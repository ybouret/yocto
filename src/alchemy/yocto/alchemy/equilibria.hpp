#ifndef YOCTO_ALCHEMY_EQUILIBRIA_INCLUDED
#define YOCTO_ALCHEMY_EQUILIBRIA_INCLUDED 1

#include "yocto/alchemy/equilibrium.hpp"
#include "yocto/container/matrix.hpp"

namespace yocto
{
    namespace alchemy
    {


        class equilibria : public equilibrium::database
        {
        public:
            explicit equilibria(const library::pointer &shlib);
            virtual ~equilibria() throw();

            const library::pointer pLib;
            const size_t           M;    //!< species
            const size_t           N;    //!< equilibria
            matrix<double>         Nu;   //!< NxM, topology
            matrix<double>         NuT;  //!< NxN, Nu'
            
            equilibrium & add( const string &name, const equilibrium_constant &K);
            equilibrium & add( const string &name, const double                K);

            void compile();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibria);
            size_t max_name_length;
            void submit( const equilibrium::pointer &eq);
            
        };
    }
}

#endif
