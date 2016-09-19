#include "yocto/ocl/program.hpp"

namespace yocto
{
    namespace ocl
    {

        template <>
        void _Program:: Retain(cl_program p) throw()
        {
            (void)clRetainProgram(p);
        }

        template <>
        void _Program:: Release(cl_program p) throw()
        {
            (void)clReleaseProgram(p);
        }



        Program:: ~Program() throw()
        {
        }

        static inline
        cl_program __ocl_create_program( const Context &context, const Sources &sources )
        {
            const cl_uint count = sources.size();
            memory::buffer_of<char *,memory::pooled> strings(count);
            memory::buffer_of<size_t,memory::pooled> lengths(count);
            for(size_t i=0,j=1;i<count;++i,++j)
            {
                const string &src = sources[j];
                strings[i] = (char *)&src[0];
                lengths[i] =  src.size();
            }

            cl_int      err  = CL_SUCCESS;
            char       **src = strings();
            cl_program  prg  = clCreateProgramWithSource(*context, count, (const char **)src, lengths(), &err);
            YOCTO_OCL_CHECK(err, "clCreateProgramWithSource" );
            return prg;
        }

        Program:: Program( const Context &context, const Sources &sources ) :
        _Program(__ocl_create_program(context,sources))
        {
        }

        Program:: Program(const Program &other ) : _Program(other) {}

    }

}
