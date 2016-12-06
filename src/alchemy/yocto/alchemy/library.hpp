#ifndef YOCTO_ALCHEMY_LIBRARY_INCLUDED
#define YOCTO_ALCHEMY_LIBRARY_INCLUDED 1

#include "yocto/alchemy/species.hpp"
#include "yocto/sequence/array.hpp"
#include "yocto/ptr/arc.hpp"

namespace yocto
{
    namespace alchemy
    {

        //! a library of species, bases on species::database
        class library : public counted_object, public species::database
        {
        public:
            typedef arc_ptr<library> pointer;

            explicit library() throw();
            virtual ~library() throw();
            library(const library &);

            species & add(const string &name, const int z); //!< create a new species
            species & add(const char   *name, const int z); //!< create a new species

            species::pointer & operator[](const string &name);
            species::pointer & operator[](const char   *name); //!< look up by name

            const species::pointer & operator[](const string &name) const; //!< look up by name
            const species::pointer & operator[](const char   *name) const; //!< look up by name

            size_t index_of(const string &name) const;
            size_t index_of(const char   *name) const;

            double osmolarity_of(  const array<double> &C) const throw();
            double molar_charge_of(const array<double> &C) const throw();
            double ionic_strength_of(const array<double> &C) const throw();

            void display( std::ostream &os ) const;
            void display( std::ostream &os, const array<double> &C) const;

            double &       value_of( const string &name, array<double> &C ) const;
            double &       value_of( const char   *name, array<double> &C ) const;
            const double & value_of( const string &name, const array<double> &C ) const;
            const double & value_of( const char   *name, const array<double> &C ) const;


        private:
            YOCTO_DISABLE_ASSIGN(library);
            size_t max_name_length;
            void prolog( std::ostream &os ) const;
            void epilog( std::ostream &os ) const;

        };

    }
}

#endif
