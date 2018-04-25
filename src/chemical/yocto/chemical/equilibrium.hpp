#ifndef YOCTO_CHEMICAL_EQUILIBRIUM_INCLUDED
#define YOCTO_CHEMICAL_EQUILIBRIUM_INCLUDED 1

#include "yocto/chemical/actor.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/math/types.hpp"
#include "yocto/sequence/addr-list.hpp"

namespace yocto
{
    namespace chemical
    {

        typedef core::list_of_cpp<actor>       actors;
        
        class equilibrium : public counted_object
        {
        public:
            typedef intr_ptr<string,equilibrium>    pointer;
            typedef set<string,pointer>             database;
            typedef math::numeric<double>::function constant;
            virtual ~equilibrium() throw();

            const string     name;
            const string  &  key() const throw();
            mutable constant K;

            bool operator()(species &sp, const int nu);
            friend std::ostream & operator<<( std::ostream &os, const equilibrium &eq);
            void display( std::ostream &os, const size_t ns) const;
            
            void check() const;

            //! fill reaction topology
            void fill( array<double> &nu, array<bool> &active) const throw();

            double computeGamma( const array<double> &C, const double Kt ) const;
            void   computeGradient( array<double> &Phi, const array<double> &C, const double Kt) const;
            int    productsStoichiometry() const throw();

        protected:
            explicit equilibrium(const string &id);
            actors  reactants;
            actors  products;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibrium);
            virtual double getK(double) const throw() = 0;
        };



        class constant_equilibrium : public equilibrium
        {
        public:
            explicit constant_equilibrium(const string &id, const double Kvalue);
            virtual ~constant_equilibrium() throw();

        private:
            const double __K;
            virtual double getK(double) const throw();
            YOCTO_DISABLE_COPY_AND_ASSIGN(constant_equilibrium);
        };
        

    }
}

#endif

