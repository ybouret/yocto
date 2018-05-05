#ifndef YOCTO_CHEMICAL_EQUILIBRIA_INCLUDED
#define YOCTO_CHEMICAL_EQUILIBRIA_INCLUDED 1

#include "yocto/chemical/equilibrium.hpp"
#include "yocto/chemical/library.hpp"
#include "yocto/container/matrix.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/math/types.hpp"

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
            matrix<double> Nu;     //!< topology    [NxM]
            matrix<double> NuT;    //!< transposed  [MxN]
            matrix<double> Phi;    //!< jacobian    [NxM]
            matrix<double> W;      //!< inv(jacobian.nuT) NxN
            vector<double> K;      //!< constants   N
            vector<double> Gamma;  //!< Gamma       N
            vector<double> xi;     //!< extent      N
            vector<double> GamEV;  //!< Gamma Exponent Values [N]
            const size_t   max_length;
            

            void compile_for(const library &lib);

            void clear() throw();

            //! initialize K and Gamma
            void initializeGamma(const array<double> &C0, const double t);

            //! compute Gamma for a computed set of K
            void updateGamma(const array<double> &C0);
            void updatePhi(const array<double> &C0);
            
            //! initialize K, Gamma and Phi 
            void   initializeGammaAndPhi(const array<double> &C0,const double t);
            void   updateGammaAndPhi(const array<double> &C0);
            double GammaToScalar() const throw();

            

            //! balance a concentration
            bool balance(array<double> &C0) throw();
            
            //! normalize a balanced concentration
            void normalize( array<double> &C0, const double t );
            
            std::ostream & spaces_for( const string &id, std::ostream &os ) const;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibria);
            double __normGamma(double alpha);
            math::numeric<double>::function normGamma;

            // from Gamma and Phi
            void compute_full_step();
        };

    }
}


#endif

