#ifndef YOCTO_ALCHEMY_EQUILIBRIUM_INCLUDED
#define YOCTO_ALCHEMY_EQUILIBRIUM_INCLUDED 1

#include "yocto/alchemy/library.hpp"
#include "yocto/functor.hpp"
#include "yocto/core/list.hpp"

namespace yocto
{
    namespace alchemy
    {



        class equilibrium : public counted_object
        {
        public:
            typedef functor<double,TL1(double)> Constant;
            typedef arc_ptr<equilibrium>        pointer;


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



            const string           name;
            const library::pointer lib;
            Constant               K;

            explicit equilibrium(const string           &the_name,
                                 const library::pointer &the_lib,
                                 const Constant         &the_K);

            virtual ~equilibrium() throw();

            void add(const string &name, const int nu);
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibrium);
            actors products;
            actors reactants;
        };

    }
}

#endif
