#ifndef YOCTO_ALCHEMY_EQUILIBRIUM_INCLUDED
#define YOCTO_ALCHEMY_EQUILIBRIUM_INCLUDED 1

#include "yocto/alchemy/library.hpp"
#include "yocto/core/list.hpp"
#include "yocto/math/types.hpp"

namespace yocto
{
    namespace alchemy
    {
        //! a species with a stoechiometric coefficient
        class actor : public object
        {
        public:
            const species::pointer sp;
            const int              nu;       //!< stoechio, not zero!
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
        typedef equilibrium_constant::callable  equilibrium_constant_callable;

        //! wrapper for true constant
        class true_equilibrium_constant : public equilibrium_constant_callable
        {
        public:
            explicit true_equilibrium_constant(const double K) throw();
            virtual ~true_equilibrium_constant() throw();
            true_equilibrium_constant(const true_equilibrium_constant &other) throw();
            const double value;
            double operator()(double) throw();

            virtual equilibrium_constant_callable * clone() const;
            static  equilibrium_constant_callable * create(const double K);

        private:
            YOCTO_DISABLE_ASSIGN(true_equilibrium_constant);
        };

        //! an equilibrium
        class equilibrium : public counted_object
        {
        public:
            typedef arc_ptr<equilibrium>        pointer;

            const string           name;
            const library::pointer lib;
            equilibrium_constant   K;

            explicit equilibrium(const string               &the_name,
                                 const library::pointer     &the_lib,
                                 const equilibrium_constant &the_K);

            explicit equilibrium(const string               &the_name,
                                 const library::pointer     &the_lib,
                                 const double                the_K);

            virtual ~equilibrium() throw();

            void add(const string &name, const int nu);

            const actors & get_products()  const throw();
            const actors & get_reactants() const throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibrium);
            actors products;
            actors reactants;
        };

    }
}

#endif
