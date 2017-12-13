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
            virtual ~Module() throw();
            
            const Input    input;

            //! get next char
            Char *get(); 

            static Module *OpenFile(const string &id);
            static Module *OpenFile(const char   *id);
            static Module *OpenSTDIN();
            static Module *OpenData(const string &id, const void *buffer, const size_t buflen );
            static Module *OpenData(const char   *id, const void *buffer, const size_t buflen );
            static Module *OpenData(const string &id, const memory::ro_buffer &buff);
            static Module *OpenData(const char   *id, const memory::ro_buffer &buff);


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Module);
        };
    }
}

#endif
