#ifndef YOCTO_LINGUA_PATTERN_MATCHER_INCLUDED
#define YOCTO_LINGUA_PATTERN_MATCHER_INCLUDED 1

#include "yocto/lingua/pattern.hpp"

namespace yocto
{
    namespace lingua
    {

        class matcher : public object
        {
        public:
            explicit matcher(pattern *p);
            explicit matcher(const char   *expr);    //!< compile expression
            explicit matcher(const string &expr);    //!< compile expression
            virtual ~matcher() throw();

            bool  full_match(const string &s);
            bool  partial_match(const string &s);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(matcher);
            pattern::ptr motif;

        public:
            token last;
        };

    }
}

#endif
