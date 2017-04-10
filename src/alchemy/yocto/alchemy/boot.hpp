#ifndef YOCTO_ALCHEMY_BOOT_INCLUDED
#define YOCTO_ALCHEMY_BOOT_INCLUDED 1

#include "yocto/alchemy/equilibria.hpp"

namespace yocto
{
    namespace alchemy
    {


        //______________________________________________________________________
        //
        // compute initial composition from constraints and
        // equilibria
        //______________________________________________________________________
        class boot
        {
        public:

            //__________________________________________________________________
            //
            //! component for a constraint
            //__________________________________________________________________
            class component
            {
            public:
                typedef set<string,component> database;

                const species::pointer        sp;
                double                        weight;

                component(const species::pointer &s, const double w) throw();
                ~component() throw();
                component(const component &) throw();
                const string &key() const throw();


            private:
                YOCTO_DISABLE_ASSIGN(component);
            };

            //__________________________________________________________________
            //
            //! linear constraint: set of components equal to value
            //__________________________________________________________________
            class constraint :
            public counted_object,
            public component::database
            {
            public:
                typedef arc_ptr<constraint> pointer;
                const library::pointer      pLib;
                const double                value;

                explicit     constraint(const library::pointer &p, const double v) throw();
                virtual     ~constraint() throw();

                void  add(const string &name, const double weight);
                void  add(const char   *name, const double weight);

                friend std::ostream & operator<<( std::ostream &, const constraint &);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(constraint);
            };



            //! create a boot loader
            explicit boot(const library::pointer &p);
            virtual ~boot() throw();


            void electroneutrality();            //!< apply electroneutrality
            void osmolarity(const double osm);   //!< set osmolarity
            void ionic_strength(const double I); //!< set ionic_strength
            void conserve(const string &name, const double C); //!< conserve species
            void conserve(const string &nameA, const string &nameB, const double C);

            constraint & create(const double value);

            friend std::ostream & operator<<(std::ostream &,const boot &);

            //! compute and return reference to eqs.C
            //const array<double> &run( equilibria &eqs, const double t=0);

            void initialize( array<double> &C0, equilibria &eqs, const double t=0);
            void solution( array<double> &C0, equilibria &eqs, const double t=0);


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(boot);
            const library::pointer      pLib;
            vector<constraint::pointer> constraints;

        };

    }
}

#endif
