#ifndef YOCTO_LANG_MODULE_INCLUDED
#define YOCTO_LANG_MODULE_INCLUDED 1

#include "yocto/lang/char-info.hpp"
#include "yocto/ios/istream.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{

    namespace Lang
    {

        typedef counted_object        Object;
        typedef arc_ptr<ios::istream> Input;


        //! a wrapper around an input to tag name/column/line
        class  Module :
        public Object,
        public CharInfo
        {
        public:

            //! default constructor
            explicit Module(const string  &id,
                            const Input   &in);

            //! open a file
            explicit Module(const string &id);
            
            //! open std input
            explicit Module();
            
            virtual ~Module() throw();
            
            const Input    input;
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Module);
        };
    }
}

#endif
