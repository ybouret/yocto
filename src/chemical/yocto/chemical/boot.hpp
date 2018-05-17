#ifndef YOCTO_CHEMICAL_BOOT_INCLUDED
#define YOCTO_CHEMICAL_BOOT_INCLUDED 1

#include "yocto/chemical/equilibria.hpp"
#include "yocto/chemical/library.hpp"
//#include "yocto/associative/set.hpp"

namespace yocto
{
    namespace chemical
    {

        //! class to handle linear constraints
        class boot
        {
        public:
            class component : public counted_object
            {
            public:
                typedef intr_ptr<string,component> pointer;
                const species::pointer sp; //!< the species
                const int               w;  //!< the weight, INTEGER
                const string &key() const throw();
                virtual ~component() throw();
                explicit component(species     &which,
                                   const double weight);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(component);
            };

            typedef set<string,component::pointer> components;

            class constraint : public counted, public components
            {
            public:
                typedef arc_ptr<constraint> pointer;
                const double value;
                explicit constraint(const double v);
                virtual ~constraint() throw();
                
                void add( species &sp, const int w);

                //! fill the array P with the weights, other positions are untouched
                double fill( array<double> &p ) const throw();
                friend std::ostream & operator<<( std::ostream  &os, const constraint &);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(constraint);
            };


            const string                name;
            vector<constraint::pointer> constraints;
            
            explicit boot(const string &id);
            virtual ~boot() throw();


            void guess(array<double>    &C0,
                       equilibria       &cs,
                       const double      t);

            constraint & create( const double value );

            void electroneutrality( library &lib );
            void conserve(const double totalC, species &sp1 );
            void conserve(const double totalC, species &sp1, species &sp2 );
            void conserve(const double totalC, species &sp1, species &sp2, species &sp3 );


            friend std::ostream & operator<<( std::ostream  &os, const boot &);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(boot);
            size_t         M;     //!< #species, from equilibria
            size_t         N;     //!< #equilibria
            size_t         Nc;    //!< number of constraints
            matrix<double> P;     //!< matrix of constraints
            vector<double> L;     //!< constraints values
            matrix<double> aP2;   //!< adjoint of     P*P'
            double         dP2;   //!< determinant of P*P'
            matrix<double> Q;     //!< orthogonal complementary of P
            vector<double> Xorg;  //!< working concentrations
            vector<double> Xtry;  //!< trial concentration
            vector<double> beta;  //!< bad concentraion
            vector<double> dX;    //!< probing step
            vector<double> V;
            vector<double> dL;
            vector<double> U;
            vector<double> p2;
            equilibria    *eqs;

            double __Balance(double alpha) throw(); // Ctry = C+alpha*dC, return Ctry^2
            double __Control(double alpha);
            math::numeric<double>::function Balance;
            math::numeric<double>::function Control;

            void clear() throw();
            void balance();
            double Error( const array<double> &XX ) throw();
        };

    }

}

#endif

