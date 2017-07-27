#ifndef YOCTO_LANG_SOURCE_INCLUDED
#define YOCTO_LANG_SOURCE_INCLUDED 1

#include "yocto/lang/module.hpp"
#include "yocto/lang/token.hpp"
#include "yocto/core/pool.hpp"

namespace yocto
{
    namespace Lang
    {

        class Source
        {
        public:
            explicit Source(const Module::Handle &hModule );
            virtual ~Source() throw();

            Char       *query();                     //!< from cache or read from top module
            void        store(Char  *ch) throw();    //!< push_front of cache
            void        store(Token &tk) throw();    //!< store a token
            void        store_copy(const Token &tk); //!< store a copy
            const Char *peek();                      //!< from cache or try to read

            void        prefetch(const size_t nmax=0); //!< if 0==nmax, read all we can

        private:

            //! for multiple sources
            class hmNode : public object
            {
            public:
                hmNode         *next;
                Module::Handle  hModule;
                hmNode(const Module::Handle &hm) throw();
                ~hmNode() throw();

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(hmNode);
            };

            //! a pool of modules
            typedef core::pool_of_cpp<hmNode> hmList;

            Token  cache;
            hmList modules;

            YOCTO_DISABLE_COPY_AND_ASSIGN(Source);
        };

    }

}

#endif
