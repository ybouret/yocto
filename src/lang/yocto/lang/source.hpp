#ifndef YOCTO_LANG_SOURCE_INCLUDED
#define YOCTO_LANG_SOURCE_INCLUDED 1

#include "yocto/lang/module.hpp"
#include "yocto/lang/token.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            class Scanner; //!< to access module
        }

        //! a source of dynamic char
        class Source
        {
        public:
            explicit Source(Module *pModule) throw();
            virtual ~Source() throw();

            Char       *query();                     //!< from cache or read from top module
            void        store(Char  *ch) throw();    //!< push_front of cache
            void        store(Token &tk) throw();    //!< store a token
            void        store_copy(const Token &tk); //!< store a copy
            const Char *peek();                      //!< from cache or try to read

            void        prefetch(const size_t nmax=0);    //!< if 0==nmax, read all we can
            void        forward(const size_t n) throw();  //!< cache.size>=n
            size_t      in_cache() const throw();
            bool        is_active();
            inline const Token & operator*() const throw() { return cache; }
            inline const Stamp &stamp() const throw() { return module->stamp; }
            
        private:
            friend class Lexical::Scanner;
            Module::Handle module;
            Token          cache;

            YOCTO_DISABLE_COPY_AND_ASSIGN(Source);
        };

    }

}

#endif
