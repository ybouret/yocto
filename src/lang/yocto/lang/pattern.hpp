#ifndef YOCTO_LANG_PATTERN_INCLUDED
#define YOCTO_LANG_PATTERN_INCLUDED 1

#include "yocto/lang/source.hpp"

namespace yocto
{
    namespace Lang
    {

        class pattern : public object
        {
        public:
            const uint32_t uuid; //!< 4 bytes ID

            virtual ~pattern() throw();

            //! the cloneable interface
            virtual pattern *clone() const  = 0;

        protected:
            explicit pattern(const uint32_t t) throw();
            pattern(const pattern &other) throw();

        private:
            YOCTO_DISABLE_ASSIGN(pattern);
        };
    }
}

#endif
