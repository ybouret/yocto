#ifndef YOCTO_ALCHEMY_EQUILIBRIA_INCLUDED
#define YOCTO_ALCHEMY_EQUILIBRIA_INCLUDED 1

#include "yocto/alchemy/equilibrium.hpp"
#include "yocto/container/matrix.hpp"
#include "yocto/sequence/vector.hpp"

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
            const size_t           M;     //!< species
            const size_t           N;     //!< equilibria
            matrix<double>         Nu;    //!< NxM, topology
            matrix<double>         NuT;   //!< Nu'
            matrix<double>         Phi;   //!< NxM, Jacobian
            vector<double>         Gamma; //!< N, equilibrium indicators
            vector<double>         K;     //!< N, equilibrium constants at a given time
            matrix<double>         Xi;    //!< Nu'*Phi, chemical Jacobian
            vector<double>         xi;    //!< chemical extent
            vector<double>         dC;    //!< delta concentration
            
            equilibrium & add( const string &name, const equilibrium_constant &K);
            equilibrium & add( const string &name, const double                K);

            void compile();

            void computeXi(const array<double> &C, const double t); // compute Gamma,K, Phi and Xi at a given time
            void updateXi(const array<double> &C); //!< update Gamma,Phi and Xi

            void normalize( array<double> &C, const double t );


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibria);
            size_t max_name_length;
            void submit( const equilibrium::pointer &eq);
            void buildXi();
        };
    }
}

#endif
