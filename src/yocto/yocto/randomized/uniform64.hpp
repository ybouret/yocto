#ifndef YOCTO_RANDOMIZED_UNIFORM64_INCLUDED
#define YOCTO_RANDOMIZED_UNIFORM64_INCLUDED 1

#include "yocto/randomized/bits.hpp"
#include "yocto/hashing/hash64.hpp"

namespace yocto {

    namespace Randomized {

        template <hashing::hash64::proc H>
        class hprng : public Bits {
        public:
            virtual ~hprng() throw() {}

            /*
             virtual void seed( Bits & s ) throw() {
             seed_ = 1;
             saux_ = s.full<uint32_t>();
             }
             */

            inline virtual uint32_t next32() throw()
            {
                uint32_t lword = saux_;
                uint32_t rword = seed_++;
                H( &lword, &rword );
                return rword;
            }

        protected:
            explicit hprng() throw() :
            Bits(0xffffffff),
            seed_( 1 ),
            saux_( 0 )
            {
            }

        private:
            hprng( const hprng & );
            hprng&operator=( const hprng & );
            uint32_t seed_;
            uint32_t saux_;
        };


#define YOCTO_RANDOM_HASH64_DECL(CLASS_NAME, HASH_NAME)     \
/* */class CLASS_NAME : public hprng<HASH_NAME> {           \
/* */    public:                                            \
/* */        explicit CLASS_NAME() throw();                 \
/* */        virtual ~CLASS_NAME() throw();                 \
/* */    private:                                           \
/* */        CLASS_NAME( const CLASS_NAME & );              \
/* */        CLASS_NAME&operator=( const CLASS_NAME & );    \
}

        YOCTO_RANDOM_HASH64_DECL(Uniform64BJ,hashing::hash64::BJ);
        YOCTO_RANDOM_HASH64_DECL(Uniform64NR,hashing::hash64::NR);

    }

}

#endif
