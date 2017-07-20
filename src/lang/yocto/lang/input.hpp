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
            //typedef arc_ptr<input>           pointer;
            typedef shared_ptr<ios::istream> module;

            virtual ~input() throw();
            explicit input(const string &tag, const module &handle);
            
            module        in;
            int           line;
            const  string name;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(input);
        };

    }
}

#endif
