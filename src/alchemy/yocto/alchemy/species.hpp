#ifndef YOCTO_ALCHEMY_SPECIES_INCLUDED
#define YOCTO_ALCHEMY_SPECIES_INCLUDED 1

#include "yocto/container/vslot.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/intr.hpp"
#include "yocto/string.hpp"
#include "yocto/associative/set.hpp"

namespace yocto
{

    namespace alchemy
    {

        //! one species: data to make chemical computation
        class species : public counted_object
        {
        public:
            virtual ~species() throw();
            explicit species(const string &id, const int zz, const size_t ii);

            const string name; //!< unique name
            const int    z;    //!< algebraic charge
            const int    zsq;  //!< z*z;
            const size_t indx; //!< index in a sequence
            vslot        data; //!< extended data

            typedef intr_ptr<string,species> pointer;
            typedef set<string,pointer>      database;

            const string & key() const throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(species);
        };
    }

}

#endif
