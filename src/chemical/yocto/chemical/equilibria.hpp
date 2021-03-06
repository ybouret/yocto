#ifndef YOCTO_CHEMICAL_EQUILIBRIA_INCLUDED
#define YOCTO_CHEMICAL_EQUILIBRIA_INCLUDED 1

#include "yocto/chemical/equilibrium.hpp"
#include "yocto/chemical/library.hpp"
#include "yocto/container/matrix.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/math/fcn/zfind.hpp"

namespace yocto
{
    namespace chemical
    {

        class equilibria :
        public equilibrium::database,
        public counted
        {
        public:
            typedef equilibrium *eqptr;
            
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

            // N dependent objects, if some equilibrium
            vector<double> Cini;   //!< for Newton's algo
            vector<double> Cend;   //!< for Newton's algo
            vector<double> step;   //!< for Newton's algo, full step
            vector<eqptr>  peqs;   //!< fast access to equilibria
            vector<double> beta;   //!< if negative active species [M]
            vector<double> errc;   //!< array to sort and sum
            matrix<double> Nu;     //!< topology    [NxM]
            matrix<double> NuT;    //!< transposed  [MxN]
            matrix<double> Nu2;    //!< Nu*NuT      [NxN];
            matrix<double> aNu2;   //!< adjoint Nu2
            vector<double> nu2;    //!< norm2 of Nu [N]
            matrix<double> Phi;    //!< jacobian    [NxM]
            matrix<double> W;      //!< inv(jacobian.nuT) NxN
            vector<double> K;      //!< constants   N
            vector<double> Gamma;  //!< Gamma       N
            vector<double> xi;     //!< extent      N
            vector<double> gev;    //!< Gamma Exponent Values [N]
            vector<double> Gnorm;  //!< for precise Gamma2Scalar
            double         dNu2;   //!< determinant(Nu2)
            const size_t   max_length;
            
            void compile_for(const library &lib);

            void clear() throw();

            //! initialize K and Gamma
            void initializeGamma(const array<double> &C0, const double t);

            //! compute Gamma for a computed set of K
            void updateGamma(const array<double> &C0) throw();
            void updatePhi(const array<double> &C0) throw();
            
            //! initialize K, Gamma and Phi 
            void   initializeGammaAndPhi(const array<double> &C0,const double t);
            void   updateGammaAndPhi(const array<double> &C0);
            double GammaToScalar() const throw();

            

            //! balance a concentration
            bool balance(array<double> &C0,bool *changed=NULL) throw();

            //! normalize a balanced concentration
            bool normalize(array<double> &C0, const double t, const bool initialize=true) throw();

            //! C0 must be normalized!!
            bool __deliver( array<double> &C0, const array<double> &delta, const double t, const bool initialize=true) throw();

            //! just damping
            bool damp( array<double> &delta, const array<double> &C0, const double t, const bool initialize=true) throw();


            //! for printing
            std::ostream & spaces_for( const string &id, std::ostream &os ) const;

            double get_scale( const size_t iEq ) const throw();

            bool mix(array<double>       &Cmix,
                     const array<double> &C0, const double V0,
                     const array<double> &C1, const double V1,
                     const double t);

            functor<void,TL2(array<double>&,double)> project;

            //! compute Newton's step@Cini, recompute |Gamma| is Cini is changed
            bool compute_step(double &GammaIni) throw();

            bool is_valid(const array<double> &C0) const throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibria);
            const equilibrium  *pEq;
            double              KEq;
            math::zfind<double> zsolve;

            double __NormGamma(double alpha);
            double __Balance(double alpha);  //!< compute Ctry with numerical control
            double __GammaEq(double alpha);
            void   __Project( array<double> &C0, const double t );

            math::numeric<double>::function NormGamma;
            math::numeric<double>::function Balance;
            math::numeric<double>::function GammaEq;

            //! try to move on singular composition
            void move( const size_t i ) throw();

            //! best move for everyone
            void sweep() throw();

        };

    }
}


#endif

