#ifndef YOCTO_LANG_MODULE_INCLUDED
#define YOCTO_LANG_MODULE_INCLUDED 1

#include "yocto/ios/istream.hpp"
#include "yocto/ptr/arc.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{

    namespace Lang
    {

        typedef counted_object  Object;
        typedef arc_ptr<string> Stamp;

        class CharInfo
        {
        public:
            CharInfo(const Stamp &st) throw();
            ~CharInfo() throw();
            CharInfo(const CharInfo &) throw();

            const int column;
            const int line;

        private:
            YOCTO_DISABLE_ASSIGN(CharInfo);
        };

        //! character position
        class CharPosition
        {
        public:
            const int column;
            const int line;
            CharPosition() throw();
            virtual ~CharPosition() throw();
            CharPosition(const CharPosition &other) throw();

            void newchar() throw();
            void newline() throw();
            
        private:
            YOCTO_DISABLE_ASSIGN(CharPosition);
        };


        //! a wrapper around an input to tag name/column/line
        class  Module :
        public Object,
        public CharPosition
        {
        public:
            typedef  arc_ptr<ios::istream> Input;

            //! default constructor
            explicit Module(const string  &id,
                            const Input   &in);

            virtual ~Module() throw();

            const Stamp stamp;
            const Input input;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Module);
        };
    }
}

#endif
