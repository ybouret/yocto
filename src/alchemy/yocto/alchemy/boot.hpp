#ifndef YOCTO_ALCHEMY_BOOT_INCLUDED
#define YOCTO_ALCHEMY_BOOT_INCLUDED 1

#include "yocto/alchemy/equilibria.hpp"

namespace yocto
{
    namespace alchemy
    {

        class boot
        {
        public:

            //! component for a constraint
            class component
            {
            public:
                const species::pointer sp;
                double                 weight;

                component(const species::pointer &s, const double w) throw();
                ~component() throw();
                component(const component &) throw();
                const string &key() const throw();

                typedef set<string,component> database;

            private:
                YOCTO_DISABLE_ASSIGN(component);
            };

            //! linear constraint: set of components equal to value
            class constraint :
            public counted_object,
            public component::database
            {            void electroneutrality();

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


            void electroneutrality();           //!< electroneutrality
            void osmolarity(const double osm);  //!< set osmolarity
            void conserve(const string &name, const double C);
            void conserve(const string &nameA, const string &nameB, const double C);


            friend std::ostream & operator<<(std::ostream &,const boot &);

            void run( equilibria &eqs, const double t);


            void run2( equilibria &eqs, const double t);


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(boot);
            const library::pointer      pLib;
            vector<constraint::pointer> constraints;
            vector<double>              start_C;
            vector<double>              delta_C;
            equilibria                 *pEqs;
            double                      call_F(double alpha);



        };

    }
}

#endif
