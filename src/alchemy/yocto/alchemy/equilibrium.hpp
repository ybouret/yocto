#ifndef YOCTO_ALCHEMY_EQUILIBRIUM_INCLUDED
#define YOCTO_ALCHEMY_EQUILIBRIUM_INCLUDED 1

#include "yocto/alchemy/library.hpp"
#include "yocto/core/list.hpp"
#include "yocto/math/fcn/drvs.hpp"

namespace yocto
{
    namespace alchemy
    {
        //! a species with a stoechiometric coefficient
        class actor : public object
        {
        public:
            const species::pointer sp;
            const size_t           id;       //!< sp->indx
            const int              nu;       //!< stoechio, not zero!
            const size_t           ev;       //!< exponent value, |nu|
            const size_t           evm;      //!< ev-1
            actor                 *next;     //!< for list
            actor                 *prev;     //!< for list
            actor(const actor &) throw();
            actor(const species::pointer &the_sp,const int the_nu) throw();
            virtual ~actor() throw();

        private:
            YOCTO_DISABLE_ASSIGN(actor);
        };
        typedef core::list_of_cpp<actor> actors;

        //! time dependant constant
        typedef math::numeric<double>::function equilibrium_constant;
        typedef math::derivative<double>        drvs_type;
      
        //! an equilibrium
        class equilibrium : public counted_object
        {
        public:
            typedef intr_ptr<string,equilibrium>        pointer;
            typedef set<string,pointer>                 database;
            
            const string                   name;
            const library::pointer         pLib;
            mutable equilibrium_constant   K;

            explicit equilibrium(const string               &the_name,
                                 const library::pointer     &the_lib,
                                 const equilibrium_constant &the_K);

            explicit equilibrium(const string               &the_name,
                                 const library::pointer     &the_lib,
                                 const double                the_K);

            virtual ~equilibrium() throw();
            const string & key() const throw();

            void add(const string &name, const int nu);

            const actors & get_products()  const throw();
            const actors & get_reactants() const throw();

            //! the equilibrium indicator
            double computeGamma(const array<double> &C,
                                const double         t,
                                double              &Kt) const throw();
            //! update from Kt
            double updateGamma(const array<double> &C,
                               const double         Kt) const throw();

            //! compute gradient
            void computeGradient(array<double>       &Phi,
                                 const array<double> &C,
                                 const double         Kt) const throw();

            friend std::ostream & operator<<( std::ostream &, const equilibrium &);

            double dtGamma(const array<double> &C,
                           const double         t,
                           const double         dt,
                           drvs_type           &drvs);

            bool   involves(const string &name) const throw();
            bool   depends_on(const equilibrium &other) const throw();
            size_t number_of_species() const throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibrium);
            actors products;
            actors reactants;
            
        };

    }
}

#endif
