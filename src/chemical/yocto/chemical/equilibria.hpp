#ifndef YOCTO_CHEMICAL_EQUILIBRIA_INCLUDED
#define YOCTO_CHEMICAL_EQUILIBRIA_INCLUDED 1

#include "yocto/chemical/equilibrium.hpp"
#include "yocto/chemical/library.hpp"
#include "yocto/container/matrix.hpp"
#include "yocto/sequence/vector.hpp"

namespace yocto
{
    namespace chemical
    {

        class equilibria :
        public equilibrium::database,
        public counted
        {
        public:

            explicit equilibria(const string &id, const size_t n=0);
            virtual ~equilibria() throw();

            equilibrium & operator()(const string &id, const double K);

            friend std::ostream & operator<<( std::ostream &, const equilibria &);

            const string   name;
            const size_t   M;    //!< species
            const size_t   N;    //!< reactions
            
            // M only dependent objects
            vector<double> C;
            vector<double> dC;
            vector<bool>   active;
            vector<double> Ctry;
            
            // N dependent objects
            vector<double> beta;   //!< if negative active species [M]
            matrix<double> nu;     //!< topology    [NxM]
            matrix<double> nuT;    //!< transposed  [MxN]
            matrix<double> nu2;    //!< nuT.nu      [MxM]
            matrix<double> Phi;    //!< jacobian    [NxM]
            matrix<double> W;      //!< inv(jacobian.nuT) NxN
            vector<double> K;      //!< constants   N
            vector<double> Gamma;  //!< Gamma       N
            vector<double> xi;     //!< extent      N

            // helper
           // math::numeric<double>::function E; //!< for balancing

            void compile_for(const library &lib);

            void clear() throw();

            //! initialize K and Gamma
            void initializeGamma(const array<double> &C0, const double t);

            //! compute Gamma for a computed set of K
            void updateGamma(const array<double> &C0);

            //! initialize K, Gamma and Phi 
            void initializeGammaAndPhi(const array<double> &C0,const double t);
            void updateGammaAndPhi(const array<double> &C0);

            //! compute LU for Phi*nu'
            bool computeW();

            //! balance a pre-loaded concentration
            bool balance();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibria);
        };

    }
}


#endif

