#ifndef YOCTO_CHEMICAL_COMPONENT_INCLUDED
#define YOCTO_CHEMICAL_COMPONENT_INCLUDED 1

#include "yocto/chemical/species.hpp"

namespace yocto
{
    namespace chemical
    {
        class component : public counted_object
        {
        public:
            typedef intr_ptr<string,component> pointer;
            
            virtual  ~component() throw();
            explicit  component( species &s, const int coef);

            const species::pointer sp;
            const int              nu;

            const string & key() const throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(component);
        };
    }
}

#endif

