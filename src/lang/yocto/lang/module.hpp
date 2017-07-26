#ifndef YOCTO_LANG_MODULE_INCLUDED
#define YOCTO_LANG_MODULE_INCLUDED 1

#include "yocto/ios/istream.hpp"
#include "yocto/ptr/arc.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{

    namespace lang
    {


        //! character position
        class t_position
        {
        public:
            const int column;
            const int line;
            t_position() throw();
            virtual ~t_position() throw();
            t_position(const t_position &other) throw();

            void newchar() throw();
            void newline() throw();
            
        private:
            YOCTO_DISABLE_ASSIGN(t_position);
        };


        //! a wrapper around an input to tag name/column/line
        class module :
        public counted_object,
        public t_position
        {
        public:
            typedef  arc_ptr<ios::istream> handle;

            //! default constructor
            explicit module(const string &id,
                            const handle &in);

            virtual ~module() throw();

            explicit module(const string &filename);
            


            const string name;
            handle       input;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(module);
        };
    }
}

#endif
