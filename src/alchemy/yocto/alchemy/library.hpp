#ifndef YOCTO_ALCHEMY_LIBRARY_INCLUDED
#define YOCTO_ALCHEMY_LIBRARY_INCLUDED 1

#include "yocto/alchemy/species.hpp"
#include "yocto/sequence/array.hpp"

namespace yocto
{
    namespace alchemy
    {

        //! a library of species, bases on species::database
        class library : public species::database
        {
        public:
            explicit library() throw();
            virtual ~library() throw();
            library(const library &);

            species & add(const string &name, const int z);
            species & add(const char   *name, const int z);

            species::pointer & operator[](const string &name);
            species::pointer & operator[](const char   *name);

            const species::pointer & operator[](const string &name) const;
            const species::pointer & operator[](const char   *name) const;

            size_t index_of(const string &name) const;
            size_t index_of(const char   *name) const;

            double osmolarity_of(  const array<double> &C) const throw();
            double molar_charge_of(const array<double> &C) const throw();
            double ionic_strength_of(const array<double> &C) const throw();

            void display( std::ostream &os ) const;

        private:
            YOCTO_DISABLE_ASSIGN(library);
            size_t max_name_length;

        };

    }
}

#endif
