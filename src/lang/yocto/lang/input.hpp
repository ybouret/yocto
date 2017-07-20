#ifndef YOCTO_LANG_INPUT_INCLUDED
#define YOCTO_LANG_INPUT_INCLUDED 1

#include "yocto/ios/istream.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/shared.hpp"
#include "yocto/ptr/arc.hpp"

namespace yocto
{
    namespace lang
    {


        class input : public counted_object
        {
        public:
            typedef shared_ptr<ios::istream> module;
            typedef arc_ptr<input>           pointer;

            virtual ~input() throw();
            explicit input(const module  &h,
                           const string  &n);

            
            module       handle;  //!< shared  istream
            const string name;    //!< istream name
            int          line;    //!< current line
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(input);
        };

    }
}

#endif
