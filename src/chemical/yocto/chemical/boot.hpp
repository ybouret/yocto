#ifndef YOCTO_CHEMICAL_BOOT_INCLUDED
#define YOCTO_CHEMICAL_BOOT_INCLUDED 1

#include "yocto/chemical/equilibria.hpp"
#include "yocto/chemical/library.hpp"
#include "yocto/associative/set.hpp"

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
                const double           w;  //!< the weight
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
                
                void add( species &sp, const double w);

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


            void guess(array<double> &C0,
                       equilibria    &cs,
                       const double   t);

            constraint & create( const double value );

            void electroneutrality( library &lib );
            void conserve(const double totalC, species &sp1 );
            void conserve(const double totalC, species &sp1, species &sp2 );
            void conserve(const double totalC, species &sp1, species &sp2, species &sp3 );


            friend std::ostream & operator<<( std::ostream  &os, const boot &);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(boot);
            const size_t   Nc;
            matrix<double> P;
            vector<double> Lam;
            vector<double> Cstar;
            void compute_Cstar();

            void clear() throw();
        };

    }

}

#endif

