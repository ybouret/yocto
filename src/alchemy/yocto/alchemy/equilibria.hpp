#ifndef YOCTO_ALCHEMY_EQUILIBRIA_INCLUDED
#define YOCTO_ALCHEMY_EQUILIBRIA_INCLUDED 1

#include "yocto/alchemy/equilibrium.hpp"
#include "yocto/container/matrix.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/math/triplet.hpp"

namespace yocto
{

    namespace alchemy
    {
        using math::triplet;

        
        class equilibria : public counted_object, public equilibrium::database
        {
        public:
            typedef arc_ptr<equilibria> pointer;

            explicit equilibria(const library::pointer &shlib);
            virtual ~equilibria() throw();

            const library::pointer pLib;
            const size_t           M;      //!< species
            const size_t           N;      //!< equilibria
            matrix<double>         Nu;     //!< NxM, topology
            matrix<double>         NuT;    //!< Nu', MxN
            matrix<double>         Nu2;    //!< Nu'*Nu, M*M
            matrix<double>         Phi;    //!< NxM, Jacobian
            vector<double>         Gamma;  //!< N, equilibrium indicators
            vector<double>         K;      //!< N, equilibrium constants at a given time
            matrix<double>         Chi;    //!< Nu'*Phi, chemical Jacobian
            vector<double>         xi;     //!< chemical extent
            vector<double>         C;      //!< local concentrations
            vector<double>         dC;     //!< delta concentration
            vector<bool>           active; //!< active species flag
            vector<double>         beta;   //!< bad concentrations for balancing, a.k.a gradient
            vector<double>         Cini;   //!< for trials
            vector<double>         step;   //!< for normalizing
            vector<double>         g;
            vector<double>         h;
            vector<double>         b;
            matrix<int>            iNu;    //!< integer Nu
            vector<int>            nuP;    //!< sum of nu for products, units of Gamma
            mutable vector<double> gam;    //!< for normalized Gamma values
            drvs_type              drvs;


            equilibrium & add( const string &name, const equilibrium_constant &K);
            equilibrium & add( const string &name, const double                K);

            void compile(); //!< allocate and compute constant terms according to the library

            void computeChi(const array<double> &C0, const double t); //!< compute Gamma,K, Phi and Chi at a given time
            void updateChi(const array<double>  &C0);                 //!< update Gamma,Phi and Chi from stored K

            void computePhi(const array<double> &C0,const double t); //!< compute Gamma, K and Phi at a given time
            void updatePhi(const array<double>  &C0);                //!< update Gamma, Phi and Chi from a stored K

            void updateGamma(const array<double> &C0); //!< update Gamma
            double Gamma2Value() const throw();        //!< return an objective function value

            //! normalize
            /**
             - at the end, Gamma is almost 0, Phi and Chi are computed @C0
             */
            void normalize(array<double> &C0,
                           const double   t);

            friend std::ostream & operator<<( std::ostream &, const equilibria &);


            bool balance() throw(); //!< balance current C, internal code

            void validate(array<double> &C0) const throw();

        private:
            math::numeric<double>::function E;
            math::numeric<double>::function F;
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibria);
            size_t max_name_length;
            void submit( const equilibrium::pointer &eq);
            void buildChi();

            double call_E(const double alpha); //!< objective function dC = C + alpha*h
            void   compute_descent(array<double> &descent,const array<double> &Ctmp); //!< descent direction of E@Ctmp
            double call_F(const double alpha); //!< objective function: C = Cini + alpha * dC, balance, updateGamma and return Gamma2Value
        };
    }
}

#endif
