#ifndef YOCTO_IPSO_WORKSPACE_INCLUDED
#define YOCTO_IPSO_WORKSPACE_INCLUDED 1

#include "yocto/ipso/fields.hpp"
#include "yocto/ipso/subset.hpp"

namespace yocto
{
    namespace ipso
    {

        //! hold different fields with the same layouts
        template <typename COORD>
        class workspace : public subset<COORD>, public field_db, public counted
        {
        public:
            static const size_t    DIM = YOCTO_IPSO_DIM_OF(COORD);

            //! create a subset with its swaps
            inline explicit workspace(const divider<COORD> &full,
                                      const size_t          rank,
                                      const size_t          layers,
                                      const COORD           pbcs) :
            subset<COORD>(full,rank,layers,pbcs,true),
            field_db(8),
            counted()
            {
            }

            //! default dtor
            inline virtual ~workspace() throw() {}

            //! helper to create a field
            template <typename FIELD>
            inline FIELD & create( const string &field_name )
            {
                return this->template build<FIELD>(field_name,this->outer);
            }

            //! helper to create a field
            template <typename FIELD>
            inline FIELD & create( const char *field_name )
            {
                return this->template build<FIELD>(field_name,this->outer);
            }
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(workspace);
        };

        
    }
}

#endif

