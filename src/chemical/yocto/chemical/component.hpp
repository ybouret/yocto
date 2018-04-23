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
            const size_t          &id;  //! sp->indx
            const int              nu;  //!< stoichiometric coef
            const size_t           ev;  //! |nu|
            const size_t           evm; //!  ev-1
            const string & key() const throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(component);
        };
        
        class actor : public object
        {
        public:
            virtual ~actor() throw();
            explicit actor( species &s, const int coef);
            
            const species::pointer sp;
            const int              nu;  //!< stoichiometric coef
            const size_t           ev;  //! |nu|
            const size_t           evm; //!  ev-1
            actor                 *next;
            actor                 *prev;
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(actor);
        };
    }
}

#endif

