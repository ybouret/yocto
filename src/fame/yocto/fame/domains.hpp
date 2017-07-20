#ifndef YOCTO_FAME_DOMAINS_INCLUDED
#define YOCTO_FAME_DOMAINS_INCLUDED 1

#include "yocto/fame/domain.hpp"

namespace yocto
{
    namespace fame
    {
        //______________________________________________________________________
        //
        //! compute all possible domains
        // TODO: optimize from rank 0 computation !
        //______________________________________________________________________
        template <typename COORD>
        class domains : public slots_of< domain<COORD> >
        {
        public:
            YOCTO_FAME_DECL_COORD;
            typedef layout<COORD> layout_type;

            
            const whole_domain<COORD> whole;      //!< the full domain
            const size_t              max_items;  //!< max inner items
            
            inline virtual ~domains() throw()
            {
            }
    
            inline explicit domains(const coord1d      user_size,
                                    const_coord       *user_cpus,
                                    const layout_type &user_full,
                                    param_coord        user_pbc) :
            slots_of< domain<COORD> >(user_size),
            whole(user_full,user_pbc),
            max_items(0)
            {
                size_t &n = (size_t &)max_items;
                for(coord1d user_rank=0;user_rank<user_size;++user_rank)
                {
                    const domain<COORD> sub(user_rank,user_size,user_cpus,user_full,user_pbc);
                    this->push_back(sub);
                    if(sub.inner.items>n) n=sub.inner.items;
                }
            }
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(domains);
        };
    }
}

#endif
