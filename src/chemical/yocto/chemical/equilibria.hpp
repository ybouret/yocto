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

            // N dependent objects
            matrix<double> nu;   //!< topology    NxM
            matrix<double> nuT;  //!< transposed  MxN
            matrix<double> Phi;  //!< jacobian    NxM
            vector<double> K;    //!< constants   N
            vector<double> Gam;  //!< Gamma       N
            vector<double> xi;   //!< extent      N


            void compile_for(const library &lib);

            void clear() throw();

            void initializeGamma(const double t);
            void updateGamma();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibria);
        };

    }
}


#endif

