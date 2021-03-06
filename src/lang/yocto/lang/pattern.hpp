#ifndef YOCTO_LANG_PATTERN_INCLUDED
#define YOCTO_LANG_PATTERN_INCLUDED 1

#include "yocto/lang/source.hpp"
#include "yocto/code/fourcc.hpp"
#include "yocto/ptr/linked.hpp"
#include "yocto/ios/ostream.hpp"

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
virtual bool             match(YOCTO_LANG_PATTERN_MATCH_ARGS) const;   \
virtual void             __viz( ios::ostream &fp ) const;              \
virtual void             __sav( ios::ostream &fp ) const;              \
virtual bool             allowsEmpty() const throw()

        //! base class for pattern matching
        class Pattern : public counted_object
        {
        public:
            typedef arc_ptr<const Pattern> Handle;


            const uint32_t uuid; //!< 4 bytes ID
            Pattern       *next; //!< for list
            Pattern       *prev; //!< for list
            void          *addr; //!< for optimizing and stuff
            
            virtual ~Pattern() throw();

            //! the cloneable interface
            virtual Pattern *clone() const  = 0;

            //! the match interface
            virtual bool match( YOCTO_LANG_PATTERN_MATCH_ARGS ) const = 0;

            //! to check
            virtual void __viz( ios::ostream &fp ) const = 0;

            //! what to write after the UUID
            virtual void __sav( ios::ostream &fp ) const = 0;

            //! check if allows empty match
            virtual bool allowsEmpty() const throw()     = 0;

            //! save binary code
            void save( ios::ostream &fp ) const;

            string toString() const;  //!< a base64 encoded binary code
            string signature() const; //!< a 8 digits digest (SHA1-32)
            
            //! to mark address
            void __mark(ios::ostream &fp) const;

            //! save a GraphViz representation
            void graphviz(const string &filename) const;

            //! simplification after a compilation
            static Pattern * Optimize( Pattern *p ) throw();

            //! reload a binary format
            static Pattern * Load(ios::istream &fp);

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
