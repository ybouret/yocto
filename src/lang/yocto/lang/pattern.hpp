#ifndef YOCTO_LANG_PATTERN_INCLUDED
#define YOCTO_LANG_PATTERN_INCLUDED 1

#include "yocto/lang/source.hpp"
#include "yocto/code/fourcc.hpp"

namespace yocto
{
    namespace Lang
    {

#define YOCTO_LANG_PATTERN_MATCH_ARGS Source &source, Token &result

#define YOCTO_LANG_PATTERN_DECL(TYPE,A,B,C,D)                          \
private: YOCTO_DISABLE_ASSIGN(TYPE);                                   \
public:                                                                \
static  const   uint32_t UUID = YOCTO_FOURCC(A,B,C,D);                 \
inline  virtual         ~TYPE()  throw() {}                            \
inline  virtual Pattern *clone() const   { return new TYPE(*this); }   \
virtual bool             match(YOCTO_LANG_PATTERN_MATCH_ARGS) const    \

        class Pattern : public counted_object
        {
        public:
            typedef arc_ptr<Pattern> Handle;
            const uint32_t uuid; //!< 4 bytes ID
            Pattern       *next; //!< for list
            Pattern       *prev; //!< for list
            
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


        typedef core::list_of_cloneable<Pattern> Patterns;
    }
}

#endif
