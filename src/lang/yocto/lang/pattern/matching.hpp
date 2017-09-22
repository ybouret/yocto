#ifndef YOCTO_LANG_PATTERN_MATCHING_INCLUDED
#define YOCTO_LANG_PATTERN_MATCHING_INCLUDED 1

#include "yocto/lang/pattern.hpp"

namespace yocto
{
    namespace Lang
    {

        class Matching
        {
        public:
            Matching(); //!< any1
            Matching(Pattern *p) throw();
            Matching(const string &expr);
            Matching(const char   *expr);
            Matching(const Matching &);
            virtual ~Matching() throw();

            void assign(Pattern *p) throw();


            bool wholly_matches(const string &line) const;
            bool partly_matches(const string &line) const;

            void swap_with( Matching &other ) throw();
            
            inline Matching & operator= ( const Matching &other )
            {
                Matching tmp(other); swap_with(tmp); return *this;
            }

            inline Matching & operator= ( const string &expr )
            {
                Matching tmp(expr); swap_with(tmp); return *this;
            }

            inline Matching & operator= ( const char  *expr )
            {
                Matching tmp(expr); swap_with(tmp); return *this;
            }

            inline Matching & operator= ( Pattern *p ) throw()
            {
                Matching tmp(p); swap_with(tmp); return *this;
            }

        private:
            Pattern::Handle motif;
            void check();
        };
    }
}

#endif
