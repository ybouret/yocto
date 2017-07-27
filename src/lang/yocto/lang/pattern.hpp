#ifndef YOCTO_LANG_PATTERN_INCLUDED
#define YOCTO_LANG_PATTERN_INCLUDED 1

#include "yocto/lang/source.hpp"

namespace yocto
{
    namespace Lang
    {

#define YOCTO_LANG_PATTERN_MATCH_ARGS Source &source, Token &result

#define YOCTO_LANG_PATTERN_INTERFACE() \
virtual Pattern *clone() const;\
virtual bool     match(YOCTO_LANG_PATTERN_MATCH_ARGS) const

        class Pattern : public counted_object
        {
        public:
            const uint32_t uuid; //!< 4 bytes ID

            virtual ~Pattern() throw();

            //! the cloneable interface
            virtual Pattern *clone() const  = 0;

            //! the match interface
            virtual bool match( YOCTO_LANG_PATTERN_MATCH_ARGS ) const = 0;

        protected:
            explicit Pattern(const uint32_t t) throw();
            Pattern(const Pattern &other) throw();

        private:
            YOCTO_DISABLE_ASSIGN(Pattern);
        };
    }
}

#endif
