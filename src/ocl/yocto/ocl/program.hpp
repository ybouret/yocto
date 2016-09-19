#ifndef YOCTO_OCL_PROGRAM_INCLUDED
#define YOCTO_OCL_PROGRAM_INCLUDED 1

#include "yocto/ocl/context.hpp"
#include "yocto/sequence/vector.hpp"

namespace yocto
{
    namespace ocl
    {
        typedef vector<string,memory::pooled::allocator> Sources;
        typedef Shared<cl_program>                      _Program;

        class Program : public _Program
        {
        public:
            virtual ~Program() throw();
            explicit Program(const Context &context, const Sources &sources);
            Program(const Program &other);
            
        private:
            YOCTO_DISABLE_ASSIGN(Program);
        };

    }
}

#endif
