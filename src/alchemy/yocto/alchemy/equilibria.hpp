#ifndef YOCTO_ALCHEMY_EQUILIBRIA_INCLUDED
#define YOCTO_ALCHEMY_EQUILIBRIA_INCLUDED 1

#include "yocto/alchemy/equilibrium.hpp"
#include "yocto/container/matrix.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/container/tuple.hpp"

namespace yocto
{
    namespace alchemy
    {

        YOCTO_TRIPLE_DECL(YOCTO_TUPLE_STANDARD,XiLimit,
                          bool,  exists,
                          double,value,
                          size_t,index);
        inline XiLimit() throw() : exists(false), value(0), index(0) {}
        inline void reset() throw() { exists = false; value = 0; index = 0; }
        YOCTO_TRIPLE_END();

        YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,XiLimits,
                          XiLimit,forward,
                          XiLimit,reverse);
        inline XiLimits() throw() : forward(), reverse() {}
        inline void reset() throw() { forward.reset(); reverse.reset(); }
        YOCTO_PAIR_END();


        class equilibria : public equilibrium::database
        {
        public:
            

            explicit equilibria(const library::pointer &shlib);
            virtual ~equilibria() throw();

            const library::pointer pLib;
            const size_t           M;      //!< species
            const size_t           N;      //!< equilibria
            matrix<double>         Nu;     //!< NxM, topology
            matrix<double>         NuT;    //!< Nu', MxM
            matrix<double>         Phi;    //!< NxM, Jacobian
            vector<double>         Gamma;  //!< N, equilibrium indicators
            vector<double>         K;      //!< N, equilibrium constants at a given time
            matrix<double>         Xi;     //!< Nu'*Phi, chemical Jacobian
            vector<double>         xi;     //!< chemical extent
            vector<double>         C;      //!< local concentrations
            vector<double>         dC;     //!< delta concentration
            vector<bool>           active; //!< active species flag
            vector<double>         bad;    //!< bad concentrations for balancing
            matrix<int>            iNu;    //!< integer Nu
            
            equilibrium & add( const string &name, const equilibrium_constant &K);
            equilibrium & add( const string &name, const double                K);

            void compile();

            void computeXi(const array<double> &C0, const double t); // compute Gamma,K, Phi and Xi at a given time
            void updateXi(const array<double>  &C0); //!< update Gamma,Phi and Xi

            void balance(); //!< balance C

            void normalize( array<double> &C0, const double t );

            friend std::ostream & operator<<( std::ostream &, const equilibria &);




        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibria);
            size_t max_name_length;
            void submit( const equilibrium::pointer &eq);
            void buildXi();
            void compute(XiLimits &limits, const array<int> &nu) const throw();
            bool try_balance(const size_t j, const XiLimits limits, const array<int> &nu) throw();

        };
    }
}

#endif
