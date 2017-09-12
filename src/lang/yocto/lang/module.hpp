#ifndef YOCTO_LANG_MODULE_INCLUDED
#define YOCTO_LANG_MODULE_INCLUDED 1

#include "yocto/lang/char.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ios/istream.hpp"

namespace yocto
{

    namespace Lang
    {

        typedef arc_ptr<ios::istream> Input;



        //! a wrapper around an input to tag stamp/column/line
        class  Module :
        public counted_object,
        public CharInfo
        {
        public:
            typedef arc_ptr<Module> Handle;
            
            //! default constructor
            explicit Module(const string  &id,
                            const Input   &in);

            //! open a file
            explicit Module(const string &id);

            //! open a data
            explicit Module(const void *buffer, const size_t buflen);

            //! open std input
            explicit Module();
            
            virtual ~Module() throw();
            
            const Input    input;

            //! get next char
            Char *get(); 

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Module);
        };
    }
}

#endif
