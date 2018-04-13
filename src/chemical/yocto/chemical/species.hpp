#ifndef YOCTO_CHEMICAL_SPECIES_INCLUDED
#define YOCTO_CHEMICAL_SPECIES_INCLUDED 1

#include "yocto/string.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/intr.hpp"

namespace yocto
{
    namespace chemical
    {
        class species : public counted_object
        {
        public:
            typedef intr_ptr<string,species> pointer;

            explicit species(const string &id,
                             const int     charge);

            virtual ~species() throw();

            const string name; //!< unique    name
            const int    z;    //!< algebraic charge
            const size_t indx; //!< index in library
            
            const string & key() const throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(species);
        };
    }
}

#endif

